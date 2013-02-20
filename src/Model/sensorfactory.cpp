#include "sensorfactory.h"

SensorFactory& SensorFactory::getInstance()
{
  static SensorFactory instance;
  return instance;
}

Sensor* SensorFactory::produce(int id,
                               double lon,
                               double lat,
                               double mos,
                               double range,
                               std::string type)
{
  Sensor* sensor = nullptr;
  sensors_map_t::const_iterator iter = sensorsMap.find(id);
  if (iter != sensorsMap.end()) // if there is already created sensor
    return iter->second;  // return it

  if (type == "camera")
    sensor = new CameraSensor(id,lon,lat,mos,range);
  else
    sensor = new Sensor(id,lon,lat,mos,range,type);

  sensorsMap.insert(std::pair<int,Sensor*>(id,sensor));
  return sensor;
}
