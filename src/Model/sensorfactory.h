#ifndef SENSORFACTORY_H
#define SENSORFACTORY_H

#include <set>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "DB/dyndbdriver.h" // DB::DynDBDriver::Sensor_row definition
#include "sensor.h"

// Uses Meyer's singleton design pattern
class SensorFactory : boost::noncopyable
     // noncopyable to disable construction out of getInstance()
{
public:
  typedef std::unordered_map<int,Sensor*> sensors_map_t;
  static SensorFactory& getInstance();

  // deletes Sensor_rows.
  virtual std::set<Sensor*>
    transformSensors(std::set<DB::DynDBDriver::Sensor_row*>);
  virtual Sensor* produce(int id,
                          double lon, double lat, double mos,
                          double range, std::string type);

private:
  sensors_map_t sensorsMap;
};

#endif // SENSORFACTORY_H
