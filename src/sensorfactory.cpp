#include "sensorfactory.h"

SensorFactory::SensorFactory()
{
}

Sensor* SensorFactory::produce(int id,
                               double lon,
                               double lat,
                               double mos,
                               double range,
                               std::string type) const
{
  if (type == "camera")
    return new CameraSensor(id,lon,lat,mos,range);
  else
    return new Sensor(id,lon,lat,mos,range,type);
}
