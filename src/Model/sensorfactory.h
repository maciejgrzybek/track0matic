#ifndef SENSORFACTORY_H
#define SENSORFACTORY_H

#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "sensor.h"

class SensorFactory : boost::noncopyable
     // noncopyable to disable construction out of getInstance()
{
public:
  typedef std::unordered_map<int,Sensor*> sensors_map_t;
  static SensorFactory& getInstance();
  virtual Sensor* produce(int id,
                          double lon,
                          double lat,
                          double mos,
                          double range,
                          std::string type);

  sensors_map_t sensorsMap;
};

#endif // SENSORFACTORY_H
