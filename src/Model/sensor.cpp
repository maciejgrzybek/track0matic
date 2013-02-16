#include <cmath>

#include "sensor.h"

Sensor::Sensor(int id,
               double lon,
               double lat,
               double mos,
               double range,
               std::string type)
  : id_(id),
    lon_(lon),
    lat_(lat),
    mos_(mos),
    range_(range),
    type_(type)
{
}

bool Sensor::isInRange(double lon, double lat, double mos) const
{
  // TODO change this to use field of view, rotation etc.

  // equation which indicates whether point is in range (3d sphere equation)
  // (X-Sx)^2 + (Y-Sy)^2 + (Z-Sz)^2 <= r^2
  double result = pow(lon-lon_,2) + pow(lat-lat_,2) + pow(mos-mos_,2);
  return (result <= pow(range_,2));
}

CameraSensor::CameraSensor(int id,
                           double lon,
                           double lat,
                           double mos,
                           double range)
  : Sensor(id,lon,lat,mos,range,"camera")
{
}
