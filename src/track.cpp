#include "track.h"

Track::Track(boost::posix_time::ptime creationTime)
{
  if (creationTime != boost::posix_time::not_a_date_time) // if creationTime is valid (is given), use it
    refreshTime_ = creationTime;
  else // otherwise - get current time
    refreshTime_ = boost::posix_time::second_clock::local_time();
}

void Track::refresh(boost::posix_time::ptime refreshTime)
{
  refreshTime_ = refreshTime;
}
