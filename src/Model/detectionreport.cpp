#include <ctime>

#include "dyndbdriver.h"
#include "feature.h"

#include "detectionreport.h"

DetectionReport::DetectionReport(const DB::DynDBDriver::DRCursor::DR_row& dr_row, const features_set_t& features)
  : features(features),
    sensor_id(dr_row.sensor_id),
    dr_id(dr_row.dr_id),
    lon(dr_row.lon),
    lat(dr_row.lat),
    mos(dr_row.mos),
    uploadTime(dr_row.upload_time),
    sensorTime(dr_row.sensor_time),
    sensor(dr_row.sensor)
{
}

DetectionReport::DetectionReport(int sensor_id, int dr_id, double lon, double lat, double mos,
                                 time_t upload_time, time_t sensor_time, Sensor* sensor, const features_set_t& features)
  : features(features),
    sensor_id(sensor_id),
    dr_id(dr_id),
    lon(lon),
    lat(lat),
    mos(mos),
    uploadTime(upload_time),
    sensorTime(sensor_time),
    sensor(sensor)
{
}

int DetectionReport::getSensorId() const
{
  return sensor_id;
}

int DetectionReport::getDrId() const
{
  return dr_id;
}

double DetectionReport::getLongitude() const
{
  return lon;
}

double DetectionReport::getLatitude() const
{
  return lat;
}

double DetectionReport::getMetersOverSea() const
{
  return mos;
}

time_types::ptime_t DetectionReport::getUploadTime() const
{
  return time_types::clock_t::from_time_t(uploadTime);
}

time_types::ptime_t DetectionReport::getSensorTime() const
{
  return time_types::clock_t::from_time_t(sensorTime);
}

Sensor* DetectionReport::getSensor() const
{
  return sensor;
}

DetectionReport::features_set_t DetectionReport::getFeatures() const
{
  return features;
}

DetectionReport::features_set_t& DetectionReport::getFeaturesRef()
{
  return features;
}

Feature* DetectionReport::getFeatureOfGivenName(const std::string& name) const
{
  // TODO can be optimized (linear vs logarithmic search)
  for (auto& feature : features)
  {
    if (feature->getName() == name)
      return feature;
  }

  return nullptr;
}


bool DetectionReport::operator==(const DetectionReport& o) const
{
  return (getSensorTime() == o.getSensorTime())
      && (getDrId() == o.getDrId());
}

std::ostream& operator<<(std::ostream& out, const DetectionReport& dr)
{
  return out << "DetectionReport: id = "<< dr.getDrId()
             << " longitude = " << dr.getLongitude()
             << " latitude = " << dr.getLatitude()
             << " meters over sea = " << dr.getMetersOverSea()
             << " upload time = " << dr.getUploadTime()
             << " sensor time = " << dr.getSensorTime();
}

