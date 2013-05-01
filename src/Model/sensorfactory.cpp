#include "sensorfactory.h"

SensorFactory& SensorFactory::getInstance()
{
  static SensorFactory instance;
  return instance;
}

std::set<Sensor*>
SensorFactory::transformSensors(std::set<DB::DynDBDriver::Sensor_row*> sensors)
{
  std::set<Sensor*> result;
  for (const DB::DynDBDriver::Sensor_row* row : sensors)
  {
    Sensor* sensor = produce(row->sensor_id,row->lon,row->lat,row->mos,
                             row->range,row->type);
    result.insert(sensor);

    delete row;
  }

  return result;
}

Sensor* SensorFactory::produce(int id,
                               double lon, double lat, double mos,
                               double range, std::string type)
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
