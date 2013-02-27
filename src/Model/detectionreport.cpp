#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "dyndbdriver.h"

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

boost::posix_time::ptime DetectionReport::getUploadTime() const
{
  return boost::posix_time::from_time_t(uploadTime);
}

boost::posix_time::ptime DetectionReport::getSensorTime() const
{
  return boost::posix_time::from_time_t(sensorTime);
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
