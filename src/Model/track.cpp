#include "detectionreport.h"

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
  // initialize estimation filter
  applyMeasurement(longitude,latitude,metersOverSea);
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

time_types::ptime_t Track::getRefreshTime() const
{
  return refreshTime_;
}

void Track::applyMeasurement(const DetectionReport& dr)
{
  return applyMeasurement(dr.getLongitude(),
                          dr.getLatitude(),
                          dr.getMetersOverSea());
}

void Track::applyMeasurement(double longitude, double latitude, double /*mos*/)
{

  estimation::EstimationFilter<>::vector_t vec;
  vec[0] = longitude;
  vec[1] = latitude;
  vec[2] = 0; // not using speed yet, TODO use speed according to current speed
  vec[3] = 0; // obtained from Track

  estimationFilter_->correct(vec);
}

bool Track::isTrackValid(time_types::ptime_t currentTime,
                         time_types::duration_t TTL) const
{
  if (currentTime - refreshTime_ <= TTL)
    return true;

  return false;
}
