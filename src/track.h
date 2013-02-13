#ifndef TRACK_H
#define TRACK_H

#include <boost/date_time/posix_time/posix_time_types.hpp>

class Track
{
public:

  /**
   * @brief c-tor. Creates track based on given creation time.
   *
   *  Creation time is given, because when working in batch mode, current time cannot be used to maintain tracks,
   *  e.g. When replaying data, tracks initialized with time in future (the simulation time) would live too long (longer than when it's real-time tracking).
   *  Default value causes setting current local time, instead of given.
   *  So, whenever you want to create Track without outside time synchronizer and the moment of creation is a moment when track appeared in system,
   *  use default argument to c-tor.
   * @param Time of track creation, needed to maintain track (remove after no-update time).
   */
  Track(boost::posix_time::ptime = boost::posix_time::ptime());

  /**
   * @brief Refreshes Track (sets it's last update time to given)
   * @param Time of refresh
   */
  void refresh(boost::posix_time::ptime refreshTime);

private:
  boost::posix_time::ptime refreshTime_;
};

#endif // TRACK_H
