#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <memory>
#include <string>

#include <Common/logger.h>
#include <Common/threadbuffer.hpp>

#include <Model/model.h>

#include <Model/DB/dyndbdriver.h>
#include <Model/reportmanager.h>
#include <Model/alignmentprocessor.h>
#include <Model/candidateselector.h>
#include <Model/dataassociator.h>
#include <Model/trackmanager.h>
#include <Model/featureextractor.h>
#include <Model/fusionexecutor.h>

#include <3rdparty/StaticBaseDriver.h>

namespace Model
{

class DataManager : public Model
{
public:
  DataManager(const std::string& paramsPath = "options.xml",
              std::shared_ptr<DB::DynDBDriver> dynDbDriver
                = std::shared_ptr<DB::DynDBDriver>(),
              std::shared_ptr<StaticBaseDriver> staticDbDriver
                = std::shared_ptr<StaticBaseDriver>(),
              std::unique_ptr<estimation::EstimationFilter<> >
                = std::unique_ptr<estimation::EstimationFilter<> >(),
              std::unique_ptr<ReportManager> reportManager
                = std::unique_ptr<ReportManager>(),
              std::unique_ptr<AlignmentProcessor> alignmentProcessor
                = std::unique_ptr<AlignmentProcessor>(),
              std::unique_ptr<CandidateSelector> candidateSelector
                = std::unique_ptr<CandidateSelector>(),
              std::shared_ptr<TrackManager> trackManager
                = std::unique_ptr<TrackManager>(),
              std::unique_ptr<DataAssociator> dataAssociator
                = std::unique_ptr<DataAssociator>(),
              std::unique_ptr<FeatureExtractor> featureExtractor
                = std::unique_ptr<FeatureExtractor>(),
              std::unique_ptr<FusionExecutor> fusionExecutor
                = std::unique_ptr<FusionExecutor>(),
              time_types::duration_t TTL = boost::chrono::seconds(0));

  /**
   * @brief Computes next state of Model.
   *
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
   *
   * @param Current time - indicates what time is it in Model right now.
   *  When default value is used, refresh time of the latest Track is used.
   * @return Snapshot - collection of Tracks after computation.
   */
  virtual Snapshot computeState(time_types::ptime_t currentTime
                                  = time_types::ptime_t());

  virtual Snapshot getSnapshot() const;

  virtual MapPtr getMap();

private:
  /**
   * @brief Executes one full step of tracking process,
   *  when computing finishes, returns one set of Tracks.
   * @param time after which, tracks without refreshing will be removed.
   * @param time used to remove expired tracks. Indicates current time.
   *  Tracks older than currentTime-TTL will be removed.
   *  If default value given,
   *  refresh time of the latest track will be assumed current.
   * @return Set of tracks after performed full Tracking process.
   */
  std::shared_ptr<
        std::set<std::shared_ptr<Track> >
      > computeTracks(time_types::duration_t TTL,
                      time_types::ptime_t currentTime = time_types::ptime_t());

  void compute();

  Snapshot cloneTracksInSnapshot(std::shared_ptr<
                                     std::set<std::shared_ptr<Track> >
                                  > tracks) const;

  void initializeKalmanFilter();

  void putTracksSnapshotIntoDB(const Snapshot&);

  /* after C++11's std::atomic_load<std::shared_ptr> will be implemented
   * we will use lock-free implementation, based on it,
   * instead of mutexed buffer, to return Snapshot.
   */
  Common::ThreadBuffer<Snapshot> snapshot_;

  std::shared_ptr<DB::DynDBDriver> dynDbDriver_;
  std::shared_ptr<StaticBaseDriver> staticDbDriver_;
  std::unique_ptr<ReportManager> reportManager_;
  std::unique_ptr<AlignmentProcessor> alignmentProcessor_;
  std::unique_ptr<CandidateSelector> candidateSelector_;
  std::unique_ptr<DataAssociator> dataAssociator_;
  std::shared_ptr<TrackManager> trackManager_;
  std::unique_ptr<FeatureExtractor> featureExtractor_;
  std::unique_ptr<FusionExecutor> fusionExecutor_;
  std::unique_ptr<estimation::EstimationFilter<> > filter_;

  time_types::duration_t TTL_;

  MapPtr staticMap_;
};

} // namespace Model

#endif // DATAMANAGER_H
