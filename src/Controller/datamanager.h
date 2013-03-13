#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <memory>
#include <string>

#include "../Model/reportmanager.h"
#include "../Model/alignmentprocessor.h"
#include "../Model/candidateselector.h"
#include "../Model/dataassociator.h"
#include "../Model/trackmanager.h"
#include "../Model/featureextractor.h"
#include "../Model/dyndbdriver.h"

class DataManager
{
public:
  DataManager(const std::string& paramsPath = "options.xml",
              std::shared_ptr<DB::DynDBDriver> dynDbDriver = std::shared_ptr<DB::DynDBDriver>(),
              std::unique_ptr<ReportManager> reportManager = std::unique_ptr<ReportManager>(),
              std::unique_ptr<AlignmentProcessor> alignmentProcessor = std::unique_ptr<AlignmentProcessor>(),
              std::unique_ptr<CandidateSelector> candidateSelector = std::unique_ptr<CandidateSelector>(),
              std::unique_ptr<DataAssociator> dataAssociator = std::unique_ptr<DataAssociator>(),
              std::unique_ptr<TrackManager> trackManager = std::unique_ptr<TrackManager>(),
              std::unique_ptr<FeatureExtractor> featureExtractor = std::unique_ptr<FeatureExtractor>(),
              std::unique_ptr<estimation::EstimationFilter<> > = std::unique_ptr<estimation::EstimationFilter<> >());

  void startSynchronousTracking();

private:
  void compute();

  std::shared_ptr<DB::DynDBDriver> dynDbDriver_;
  std::unique_ptr<ReportManager> reportManager_;
  std::unique_ptr<AlignmentProcessor> alignmentProcessor_;
  std::unique_ptr<CandidateSelector> candidateSelector_;
  std::unique_ptr<DataAssociator> dataAssociator_;
  std::unique_ptr<TrackManager> trackManager_;
  std::unique_ptr<FeatureExtractor> featureExtractor_;
  std::unique_ptr<estimation::EstimationFilter<> > filter_;
};

#endif // DATAMANAGER_H
