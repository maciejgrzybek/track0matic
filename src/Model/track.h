#ifndef TRACK_H
#define TRACK_H

#include <memory>
#include <unordered_set>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "estimationfilter.hpp"

class Track
{
public:
  typedef std::unordered_set<class Feature*> features_set_t;

  /**
   * @brief c-tor. Creates track based on given creation time.
   *
   *  Creation time is given, because when working in batch mode, current time cannot be used to maintain tracks,
   *  e.g. When replaying data, tracks initialized with time in future (the simulation time) would live too long (longer than when it's real-time tracking).
   *  Default value causes setting current local time, instead of given.
   *  So, whenever you want to create Track without outside time synchronizer and the moment of creation is a moment when track appeared in system,
   *  use default argument to c-tor.
   * @param Estimation filter to use, when predicting next state of Track
   * @param Time of track creation, needed to maintain track (remove after no-update time).
   */
  Track(std::unique_ptr<estimation::EstimationFilter<> > filter,
        boost::posix_time::ptime = boost::posix_time::ptime());

  /**
   * @brief Refreshes Track (sets it's last update time to given)
   * @param Time of refresh
   */
  void refresh(boost::posix_time::ptime refreshTime);


  /**
   * @brief Sets estimation filter to be used when predicting next state of track.
   * @param Filter to be assigned to track. Ownership is transferred to track.
   */
  void setEstimationFilter(std::unique_ptr<estimation::EstimationFilter<> > filter);

private:
  features_set_t features_;
  std::unique_ptr<estimation::EstimationFilter<> > estimationFilter_;
  boost::posix_time::ptime refreshTime_;
};

class HumanTrack : public Track
{
  // TODO implement this
};

class VehicleTrack : public Track
{
  // TODO implement this
};

#endif // TRACK_H
