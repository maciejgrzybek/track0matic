#include <chrono>
#include <thread>

#include "datamanager.h"

DataManager::DataManager(const std::string& paramsPath,
                         std::shared_ptr<DB::DynDBDriver> dynDbDriver,
                         std::unique_ptr<ReportManager> reportManager,
                         std::unique_ptr<AlignmentProcessor> alignmentProcessor,
                         std::unique_ptr<CandidateSelector> candidateSelector,
                         std::unique_ptr<DataAssociator> dataAssociator,
                         std::unique_ptr<TrackManager> trackManager,
                         std::unique_ptr<FeatureExtractor> featureExtractor,
                         std::unique_ptr<FusionExecutor> fusionExecutor,
                         std::unique_ptr<estimation::EstimationFilter<> > filter)
{
  if (dynDbDriver)
    dynDbDriver_ = dynDbDriver;
  else
    dynDbDriver_ = std::make_shared<DB::DynDBDriver>(paramsPath);

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
          new AlignmentProcessor(boost::posix_time::seconds(1)) // TODO read params from params file (options.xml)
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

  if (filter)
    filter_ = std::move(filter);
  else
    filter_ = std::unique_ptr<estimation::EstimationFilter<> >(
          new estimation::KalmanFilter<>()
          );
}

void DataManager::startSynchronousTracking()
{
  // TODO change this to return proper information to View
  while (true)
  {
    compute(); // loops through data flow, to maintain tracking process

    // print result of processing
    const std::set<std::shared_ptr<Track> >& tracks
        = trackManager_->getTracksRef();

    std::cout << "Tracks:" << std::endl;
    for (auto t : tracks)
    {
      std::cout << "Track: " << t->getLongitude() <<","<<t->getLatitude() << std::endl;
    }

    std::chrono::milliseconds duration(1000);
    std::this_thread::sleep_for(duration);
  }
}

void DataManager::compute()
{
  std::set<DetectionReport> DRs = reportManager_->getDRs();
  while (!DRs.empty())
  {
    alignmentProcessor_->setDRsCollection(DRs);
    std::set<DetectionReport> alignedGroup
        = alignmentProcessor_->getNextAlignedGroup();
    while (!alignedGroup.empty())
    {
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
