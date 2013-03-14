#include "track.h"

Track::Track(std::unique_ptr<estimation::EstimationFilter<> > filter,
             double longitude, double latitude, double metersOverSea,
             time_types::ptime_t creationTime)
  : estimationFilter_(std::move(filter)),
    lon_(longitude),
    lat_(latitude),
    mos_(metersOverSea),
    refreshTime_(creationTime)
{
}

void Track::refresh(time_types::ptime_t refreshTime)
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

estimation::EstimationFilter<>& Track::getEstimationFilter() const
{
  return *estimationFilter_;
}

bool Track::isTrackValid(time_types::ptime_t currentTime, time_types::duration_t TTL) const
{
// TODO implement this
}
