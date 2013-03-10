#include "track.h"

Track::Track(std::unique_ptr<estimation::EstimationFilter<> > filter,
             double longitude, double latitude, double metersOverSea,
             boost::posix_time::ptime creationTime)
  : estimationFilter_(std::move(filter)),
    lon_(longitude),
    lat_(latitude),
    mos_(metersOverSea)
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

void Track::setEstimationFilter(std::unique_ptr<estimation::EstimationFilter<> > filter)
{
  estimationFilter_ = std::move(filter);
}

Track::features_set_t Track::getFeatures() const
{
  return features_;
}

const Track::features_set_t& Track::getFeaturesRef() const
{
  return features_;
}

double Track::getLongitude() const
{
  return lon_;
}

double Track::getLatitude() const
{
  return lat_;
}

double Track::getMetersOverSea() const
{
  return mos_;
}
