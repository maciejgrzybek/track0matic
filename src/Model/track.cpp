#include "detectionreport.h"

#include "track.h"

Track::Track(std::unique_ptr<estimation::EstimationFilter<> > filter,
             double longitude, double latitude, double metersOverSea,
             double lonVar, double latVar, double mosVar,
             time_types::ptime_t creationTime)
  : estimationFilter_(std::move(filter)),
    lon_(longitude),
    lat_(latitude),
    mos_(metersOverSea),
    refreshTime_(creationTime)
{
  initializeFilter(lon_,lat_,mos_,lonVar,latVar,mosVar);
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

std::tuple<double,double,double> Track::getPredictedState() const
{
  return std::make_tuple(predictedLon_,predictedLat_,predictedMos_);
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

void Track::applyMeasurement(double longitude, double latitude, double mos)
{

  estimation::EstimationFilter<>::vector_t vec
      = coordsToStateVector(longitude,latitude,mos);

  std::pair<
        estimation::EstimationFilter<>::vector_t,
        estimation::EstimationFilter<>::vector_t
      > correctedState = estimationFilter_->correct(vec);
  estimation::EstimationFilter<>::vector_t trackCorrectedState
      = correctedState.first;

  lon_ = trackCorrectedState[0];
  lat_ = trackCorrectedState[1];

  std::pair<
        estimation::EstimationFilter<>::vector_t,
        estimation::EstimationFilter<>::vector_t
      > predictedState = estimationFilter_->predict();
  estimation::EstimationFilter<>::vector_t trackPredictedState
      = predictedState.first;

  predictedLon_ = trackPredictedState[0];
  predictedLat_ = trackPredictedState[1];
}

bool Track::isTrackValid(time_types::ptime_t currentTime,
                         time_types::duration_t TTL) const
{
  if (currentTime - refreshTime_ <= TTL)
    return true;

  return false;
}

std::unique_ptr<Track> Track::clone() const
{
  std::unique_ptr<Track> track(new Track(*this));
}

Track::Track(const Track& other)
  : estimationFilter_(std::move(other.estimationFilter_->clone())),
    lon_(other.lon_),
    lat_(other.lat_),
    mos_(other.mos_),
    refreshTime_(other.refreshTime_)
{}

estimation::EstimationFilter<>::vector_t
  Track::coordsToStateVector(double longitude,
                             double latitude,
                             double /*metersoversea*/) const
{
  // Metersoversea not yet implemented, because of too narrow state model
  // Expand model to include meters over sea value.

  estimation::EstimationFilter<>::vector_t state;
  state[0] = longitude;
  state[1] = latitude;
  state[2] = 0; //not using speed yet, TODO use speed according to current speed
  state[3] = 0; // obtained from Track

  return state;
}

void Track::initializeFilter(double longitude,
                             double latitude,
                             double metersoversea,
                             double varLon,
                             double varLat,
                             double varMos)
{
  estimation::EstimationFilter<>::vector_t state
      = coordsToStateVector(longitude,
                            latitude,
                            metersoversea);

  estimation::EstimationFilter<>::vector_t covErr // uses the same method,
      = coordsToStateVector(varLon, // because output vector has the same layout
                            varLat,
                            varMos);

  return estimationFilter_->initialize(state,covErr);
}
