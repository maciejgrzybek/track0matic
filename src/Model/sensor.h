#ifndef SENSOR_H
#define SENSOR_H

#include <string>

//TODO implement this
class Sensor
{
public:
  Sensor(int id,
         double lon,
         double lat,
         double mos,
         double range,
         std::string type);

  /**
   * @brief Checks whether point of given coordinates is in range of sensor.
   *  Default implementation assumes sensor can see around,
   *  with radius set by range.
   *
   * @param longtitude
   * @param latitude
   * @param meters over sea
   * @return
   */
  virtual bool isInRange(double lon, double lat, double mos) const;

protected:
  int id_;
  double lon_;
  double lat_;
  double mos_;
  double range_;

private:
  std::string type_;
};

class CameraSensor : public Sensor
{
public:
  CameraSensor(int id,
               double lon,
               double lat,
               double mos,
               double range);
};

#endif // SENSOR_H
