#include "datamanager.h"

DataManager::DataManager(const std::string& paramsPath,
                         std::unique_ptr<DB::DynDBDriver> dynDbDriver,
                         std::unique_ptr<ReportManager> reportManager,
                         std::unique_ptr<AlignmentProcessor> alignmentProcessor,
                         std::unique_ptr<CandidateSelector> candidateSelector,
                         std::unique_ptr<DataAssociator> dataAssociator,
                         std::unique_ptr<TrackManager> trackManager,
                         std::unique_ptr<FeatureExtractor> featureExtractor)
{
  if (dynDbDriver)
    dynDbDriver_ = std::move(dynDbDriver);
  else
    dynDbDriver = std::unique_ptr<DB::DynDBDriver>(new DB::DynDBDriver(paramsPath));

  if (reportManager)
    reportManager_ = std::move(reportManager);
  else
    reportManager_ = std::unique_ptr<ReportManager>(new ReportManager(*dynDbDriver));

  if (alignmentProcessor)
    alignmentProcessor_ = std::move(alignmentProcessor);
  else
    alignmentProcessor_ = std::unique_ptr<AlignmentProcessor>(
          new AlignmentProcessor(boost::posix_time::seconds(1)) // TODO read params from params file (options.xml)
          );

  if (candidateSelector)
    candidateSelector_ = std::move(candidateSelector);
  else
    candidateSelector_ = std::unique_ptr<CandidateSelector>(new CandidateSelector(*dynDbDriver));

  if (dataAssociator)
    dataAssociator_ = std::move(dataAssociator);
  else
    dataAssociator_ = std::unique_ptr<DataAssociator>(new DataAssociator());

  if (trackManager)
    trackManager_ = std::move(trackManager);
  else
    trackManager_ = std::unique_ptr<TrackManager>(new TrackManager());

  if (featureExtractor)
    featureExtractor_ = std::move(featureExtractor);
  else
    featureExtractor_ = std::unique_ptr<FeatureExtractor>(new FeatureExtractor());
}
