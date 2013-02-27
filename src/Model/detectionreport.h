#ifndef DETECTIONREPORT_H
#define DETECTIONREPORT_H

#include <ctime>
#include <unordered_set>

#include "dyndbdriver.h"

class Feature;
class Sensor;

class DetectionReport
{
public:
  typedef std::unordered_set<Feature*> features_set_t;

  DetectionReport(const DB::DynDBDriver::DRCursor::DR_row&,
                  const features_set_t& features = features_set_t());
  DetectionReport(int sensor_id, int dr_id, double lon, double lat, double mos,
                  time_t upload_time, time_t sensor_time, Sensor* sensor = nullptr,
                  const features_set_t& features = features_set_t());

  int getSensorId() const;
  int getDrId() const;
  double getLongitude() const;
  double getLatitude() const;
  double getMetersOverSea() const;
  boost::posix_time::ptime getUploadTime() const;
  boost::posix_time::ptime getSensorTime() const;
  Sensor* getSensor() const;

  features_set_t getFeatures() const;
  features_set_t& getFeaturesRef();

protected:
  features_set_t features;

private:
  int sensor_id;
  int dr_id;
  double lon;
  double lat;
  double mos;
  time_t uploadTime;
  time_t sensorTime;
  Sensor* sensor;
};

class HumanDR : public DetectionReport
{
 // TODO implement this when needed
};

class VehicleDR : public DetectionReport
{
 // TODO implement this when needed
};

namespace std
{

template <>
struct less<DetectionReport>
{
  bool operator()(const DetectionReport& l, const DetectionReport& r) const
  {
    boost::posix_time::ptime lTime = l.getSensorTime();
    boost::posix_time::ptime rTime = r.getSensorTime();
    if (lTime < rTime)
      return true;
    else if (lTime == rTime)
      return l.getDrId() < r.getDrId();
    else
      return false;
  }
};

} // namespace std

#endif // DETECTIONREPORT_H
