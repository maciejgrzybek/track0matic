#ifndef TRACK_H
#define TRACK_H

#include <memory>
#include <unordered_set>

#include <Common/time.h>

#include "estimationfilter.hpp"

class DetectionReport;

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
   *  Initializes estimation filter with given longitude, latitude and metersOverSea.
   * @param Estimation filter to use, when predicting next state of Track
   * @param longitude of Track
   * @param latitude of Track
   * @param meters over sea position of Track
   * @param variance of longitude (measurement of certainty)
   * @param variance of latitude (measurement of certainty)
   * @param variance of meters over sea position of Track (measurement of certainty)
   * @param Time of track creation, needed to maintain track (remove after no-update time).
   */
  Track(std::unique_ptr<estimation::EstimationFilter<> > filter,
        double longitude, double latitude, double metersOverSea,
        double lonVar, double latVar, double mosVar,
        time_types::ptime_t = time_types::clock_t::now());

  /**
   * @brief Refreshes Track (sets it's last update time to given)
   * @param Time of refresh
   */
  void refresh(time_types::ptime_t refreshTime = time_types::clock_t::now());

  /**
   * @brief Sets estimation filter to be used when predicting next state of track.
   * @param Filter to be assigned to track. Ownership is transferred to track.
   */
  void setEstimationFilter(std::unique_ptr<estimation::EstimationFilter<> > filter);

  features_set_t getFeatures() const;
  const features_set_t& getFeaturesRef() const;

  double getLongitude() const;
  double getLatitude() const;
  double getMetersOverSea() const;

  std::tuple<double,double,double> getPredictedState() const;

  time_types::ptime_t getRefreshTime() const;

  /**
   * @brief Puts model state of given DR to Track's estimation filter
   *  It's invoking correct() method on EstimationFilter assigned to Track,
   *  with values corresponding to the measured.
   * @param DetectionReport representing measurement
   */
  void applyMeasurement(const DetectionReport&);

  /**
   * @brief Puts appropriate data into model state in EstimationFilter.
   * @param longitude
   * @param latitude
   * @param meters over sea
   * @overload applyMeasurement(const DetectionReport&);
   */
  void applyMeasurement(double longitude, double latitude, double mos);

  bool isTrackValid(time_types::ptime_t currentTime,
                    time_types::duration_t TTL) const;

private:
  estimation::EstimationFilter<>::vector_t
    coordsToStateVector(double longitude,
                        double latitude,
                        double metersoversea) const;

  void initializeFilter(double longitude,
                        double latitude,
                        double metersoversea,
                        double varLon,
                        double varLat,
                        double varMos);

  double lon_;
  double lat_;
  double mos_;

  double predictedLon_;
  double predictedLat_;
  double predictedMos_; // not yet implemented

  features_set_t features_;
  std::unique_ptr<estimation::EstimationFilter<> > estimationFilter_;
  time_types::ptime_t refreshTime_;
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
