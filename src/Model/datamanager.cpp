#include <chrono>
#include <thread>

#include <Common/logger.h>

#include "datamanager.h"

namespace Model
{

DataManager::DataManager(const std::string& paramsPath,
                         std::shared_ptr<DB::DynDBDriver> dynDbDriver,
                         std::unique_ptr<estimation::EstimationFilter<> > filter,
                         std::unique_ptr<ReportManager> reportManager,
                         std::unique_ptr<AlignmentProcessor> alignmentProcessor,
                         std::unique_ptr<CandidateSelector> candidateSelector,
                         std::unique_ptr<DataAssociator> dataAssociator,
                         std::unique_ptr<TrackManager> trackManager,
                         std::unique_ptr<FeatureExtractor> featureExtractor,
                         std::unique_ptr<FusionExecutor> fusionExecutor)
{
  if (dynDbDriver)
    dynDbDriver_ = dynDbDriver;
  else
    dynDbDriver_ = std::make_shared<DB::DynDBDriver>(paramsPath);

  if (filter)
    filter_ = std::move(filter);
  else
  {
    initializeKalmanFilter();
  }

  if (reportManager)
    reportManager_ = std::move(reportManager);
  else
    reportManager_ = std::unique_ptr<ReportManager>(
          new ReportManager(dynDbDriver_)
          );

  if (alignmentProcessor)
    alignmentProcessor_ = std::move(alignmentProcessor);
  else
    alignmentProcessor_ = std::unique_ptr<AlignmentProcessor>(
          new AlignmentProcessor(boost::chrono::seconds(1)) // TODO read params from params file (options.xml)
          );

  if (candidateSelector)
    candidateSelector_ = std::move(candidateSelector);
  else
    candidateSelector_ = std::unique_ptr<CandidateSelector>(
          new CandidateSelector(dynDbDriver_)
          );

  {
    std::unique_ptr<TrackManager> tm;
    if (trackManager)
        tm = std::move(trackManager);
      else
        tm = std::unique_ptr<TrackManager>(new TrackManager(1));

    if (dataAssociator)
      dataAssociator_ = std::move(dataAssociator);
    else
    {
      std::unique_ptr<ResultComparator> resultComparator(
            new OrComparator(ResultComparator::feature_grade_map_t()));
      std::unique_ptr<ListResultComparator> listComparator(
            new OrListComparator());
      DataAssociator* da = new DataAssociator(std::move(tm),
                                              std::move(resultComparator),
                                              std::move(listComparator));
      dataAssociator_ = std::unique_ptr<DataAssociator>(da);
    }
  }

  if (trackManager)
    trackManager_ = std::move(trackManager);
  else
    trackManager_ = std::unique_ptr<TrackManager>(
          new TrackManager(0.1)
          );

  if (featureExtractor)
    featureExtractor_ = std::move(featureExtractor);
  else
    featureExtractor_ = std::unique_ptr<FeatureExtractor>(
          new FeatureExtractor()
          );

  if (fusionExecutor)
    fusionExecutor_ = std::move(fusionExecutor);
  else
    fusionExecutor_ = std::unique_ptr<FusionExecutor>(
          new FusionExecutor()
          );
}

Snapshot DataManager::computeState()
{
  Common::GlobalLogger& logger = Common::GlobalLogger::getInstance();
  logger.log("DataManager","Computing state.");
  auto tracks = computeTracks();
  // clone Tracks, to ensure safety in multithreaded environment
  Snapshot s = cloneTracksInSnapshot(tracks);
  snapshot_.put(s);
  return s;
}

Snapshot DataManager::getSnapshot() const
{
  return snapshot_.get();
}

std::shared_ptr<
      std::set<std::shared_ptr<Track> >
    >
  DataManager::computeTracks()
{
  compute(); // loops through data flow, to maintain tracking process

  trackManager_->removeExpiredTracks(boost::chrono::seconds(3)); // TODO parametrize this!

  // create new set of Tracks on heap
  //  and initialize it with tracks from TrackManager
  std::shared_ptr<
        std::set<std::shared_ptr<Track> >
      > tracks(
        new std::set<std::shared_ptr<Track> >(
          trackManager_->getTracks()
          )
        );

  return tracks; // return tracks to Controller/View
}

void DataManager::compute()
{
  Common::GlobalLogger& logger = Common::GlobalLogger::getInstance();
  std::set<DetectionReport> DRs = reportManager_->getDRs();

  { // TODO rewrite this, when logger will be more sophisticated
    std::stringstream msg;
    msg << "Retrieved " << DRs.size() << " detection reports.";
    logger.log("DataManager",msg.str());
  }

  while (!DRs.empty())
  {
    alignmentProcessor_->setDRsCollection(DRs);
    std::set<DetectionReport> alignedGroup
        = alignmentProcessor_->getNextAlignedGroup();

    while (!alignedGroup.empty())
    {
      { // TODO rewrite this, when logger will be more sophisticated
        std::stringstream msg;
        msg << "Generated aligned group of " << alignedGroup.size()
            << " detection reports.";
        logger.log("DataManager",msg.str());
      }

      std::vector<std::set<DetectionReport> > DRsGroups
          = candidateSelector_->getMeasurementGroups(alignedGroup);

      dataAssociator_->setInput(DRsGroups);
      std::map<std::shared_ptr<Track>,std::set<DetectionReport> > associated
          = dataAssociator_->getDRsForTracks();
      std::vector<std::set<DetectionReport> > notAssociated
          = dataAssociator_->getNotAssociated();

      std::unique_ptr<estimation::EstimationFilter<> > filter(
            filter_->clone());

      std::map<std::shared_ptr<Track>,std::set<DetectionReport> > initialized
          = trackManager_->initializeTracks(notAssociated,std::move(filter));
      // here we have Tracks:
      // associated - for these DRs which matched existing Tracks
      // initialized - for these DRs which didn't match existing Tracks
      //  (new Tracks were created)

      fusionExecutor_->fuseDRs(associated);
      fusionExecutor_->fuseDRs(initialized);

      alignedGroup = alignmentProcessor_->getNextAlignedGroup();
    }

    DRs = reportManager_->getDRs(); // get next group
  }
}

Snapshot DataManager::cloneTracksInSnapshot(std::shared_ptr<
                                             std::set<std::shared_ptr<Track> >
                                            > tracks) const
{
  std::shared_ptr<
      std::set<std::unique_ptr<Track> >
      > result(new std::set<std::unique_ptr<Track> >());
  const std::set<std::shared_ptr<Track> >& s = *tracks;
  for (auto t : s)
  { // for each track from set
    result->insert(t->clone());
  }
  return result;
}

void DataManager::initializeKalmanFilter()
{
  estimation::KalmanFilter<>::Matrix A(4,4);
  /*
   * 1 0 1 0
   * 0 1 0 1
   * 0 0 1 0
   * 0 0 0 1
   */
  A.clear();
  A(0,0) = 1;
  A(0,1) = 0;
  A(0,2) = 1;
  A(0,3) = 0;

  A(1,0) = 0;
  A(1,1) = 1;
  A(1,2) = 0;
  A(1,3) = 1;

  A(2,0) = 0;
  A(2,1) = 0;
  A(2,2) = 1;
  A(2,3) = 0;

  A(3,0) = 0;
  A(3,1) = 0;
  A(3,2) = 0;
  A(3,3) = 1;

  estimation::KalmanFilter<>::Matrix B;
  B.clear();

  estimation::KalmanFilter<>::Matrix R(2,2);
  /*
   * 100 0.00000
   * 0.00000 100
   */
  R.clear();
  R(0,0) = 100;
  R(1,1) = 100;

  estimation::KalmanFilter<>::Matrix Q(4,4);
  /*
   * 0.01 0.00 0.00 0.00
   * 0.00 0.01 0.00 0.00
   * 0.00 0.00 0.01 0.00
   * 0.00 0.00 0.00 0.01
   */
  Q.clear();
  Q(0,0) = 0.01;
  Q(1,1) = 0.01;
  Q(2,2) = 0.01;
  Q(3,3) = 0.01;

  estimation::KalmanFilter<>::Matrix H(2,4);
  /*
   * 1 0 1 0
   * 0 1 0 1
   */
  H.clear();
  H(0,0) = 1;
  H(0,1) = 0;
  H(0,2) = 1;
  H(0,3) = 0;
  H(1,0) = 0;
  H(1,1) = 1;
  H(1,2) = 0;
  H(1,3) = 1;

  filter_ = std::unique_ptr<estimation::KalmanFilter<> >(
        new estimation::KalmanFilter<>(A,B,R,Q,H));
}

} // namespace Model
