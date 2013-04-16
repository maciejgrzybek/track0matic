#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <memory>
#include <string>

#include <Common/logger.h>
#include <Common/threadbuffer.hpp>

#include <Model/model.h>

#include <Model/dyndbdriver.h>
#include <Model/reportmanager.h>
#include <Model/alignmentprocessor.h>
#include <Model/candidateselector.h>
#include <Model/dataassociator.h>
#include <Model/trackmanager.h>
#include <Model/featureextractor.h>
#include <Model/fusionexecutor.h>

namespace Model
{

class DataManager : public Model
{
public:
  DataManager(const std::string& paramsPath = "options.xml",
              std::shared_ptr<DB::DynDBDriver> dynDbDriver
                = std::shared_ptr<DB::DynDBDriver>(),
              std::unique_ptr<estimation::EstimationFilter<> >
                = std::unique_ptr<estimation::EstimationFilter<> >(),
              std::unique_ptr<ReportManager> reportManager
                = std::unique_ptr<ReportManager>(),
              std::unique_ptr<AlignmentProcessor> alignmentProcessor
                = std::unique_ptr<AlignmentProcessor>(),
              std::unique_ptr<CandidateSelector> candidateSelector
                = std::unique_ptr<CandidateSelector>(),
              std::unique_ptr<DataAssociator> dataAssociator
                = std::unique_ptr<DataAssociator>(),
              std::unique_ptr<TrackManager> trackManager
                = std::unique_ptr<TrackManager>(),
              std::unique_ptr<FeatureExtractor> featureExtractor
                = std::unique_ptr<FeatureExtractor>(),
              std::unique_ptr<FusionExecutor> fusionExecutor
                = std::unique_ptr<FusionExecutor>());

  /**
   * In multithreaded environment,
   * there is no guarantee the value returned from computeState()
   * will be the same as this from getSnapshot().
   * Value returned by computeState()
   *  is ALWAYS the value computed by this invocation.
   * Value returned by getSnapshot() is last value stored in mutexed buffer,
   *  but be aware, that computeState() does not have to finish it's work,
   *  to store snapshot in buffer! But if computeState() finished work,
   *  there is a certainty the computed value (state),
   *  at least WAS (or maybe is still there) for a while in buffer.
   */
  virtual Snapshot computeState();
  virtual Snapshot getSnapshot() const;

private:
  /**
   * @brief Executes one full step of tracking process,
   *  when computing finishes, returns one set of Tracks.
   *
   * @return Set of tracks after performed full Tracking process.
   */
  std::shared_ptr<
        std::set<std::shared_ptr<Track> >
      > computeTracks();

  void compute();

  Snapshot cloneTracksInSnapshot(std::shared_ptr<
                                     std::set<std::shared_ptr<Track> >
                                  > tracks) const;

  void initializeKalmanFilter();

  /* after C++11's std::atomic_load<std::shared_ptr> will be implemented
   * we will use lock-free implementation, based on it,
   * instead of mutexed buffer, to return Snapshot.
   */
  Common::ThreadBuffer<Snapshot> snapshot_;

  std::shared_ptr<DB::DynDBDriver> dynDbDriver_;
  std::unique_ptr<ReportManager> reportManager_;
  std::unique_ptr<AlignmentProcessor> alignmentProcessor_;
  std::unique_ptr<CandidateSelector> candidateSelector_;
  std::unique_ptr<DataAssociator> dataAssociator_;
  std::unique_ptr<TrackManager> trackManager_;
  std::unique_ptr<FeatureExtractor> featureExtractor_;
  std::unique_ptr<FusionExecutor> fusionExecutor_;
  std::unique_ptr<estimation::EstimationFilter<> > filter_;
};

} // namespace Model

#endif // DATAMANAGER_H
