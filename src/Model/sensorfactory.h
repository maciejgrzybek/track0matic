#ifndef SENSORFACTORY_H
#define SENSORFACTORY_H

#include "sensor.h"

class SensorFactory
{
public:
  SensorFactory();
  virtual Sensor* produce(int id,
                          double lon,
                          double lat,
                          double mos,
                          double range,
                          std::string type) const;
};

#endif // SENSORFACTORY_H
