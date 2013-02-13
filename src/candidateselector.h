#ifndef CANDIDATESELECTOR_H
#define CANDIDATESELECTOR_H

#include "sensor.h"
#include "detectionreport.h"
#include "dyndbdriver.h"

// TODO do not group DRs from sensors from neighborhood (basis only on sensors neighborhood),
//  instead of this, try to figure out which DR could be seen by (more) sensors and group these.
// It's important because in some cases, there could be sensors qualified to be in neighborhood
// (e.g. because of partial common coverage of view), but in fact DR could not be seen by 2 sensors from this kind of neighborhood.
// In such a case, it's important to check DRs each other for neighborhood, according to their sensors details.
// Warning! Consider complexity of such operation, maybe it's not worth spending so much time calculating "improvemed" version, which gives almost zero impact on final result
class CandidateSelector
{
public:
  /**
   * @brief c-tor loads sensors descriptions (from DynDBDriver).
   * @param Reference to Dynamic DB Driver, which will be used to get sensors descriptions
   */
  CandidateSelector(DB::DynDBDriver& dbdriver);

  /**
   * @brief Method groups DRs by characteristics of sensor,
   *  like it's range and location.
   *
   *  Each returned element contains collection of DRs from sensors from neighborhood.
   *  To determine neighborhood, range and location of sensors are used.
   *  Each DR
   * @return collection of collection of DRs
   */
  std::vector<std::vector<DetectionReport> >
  getMeasurementGroups(const std::vector<DetectionReport>&) const;

private:
  DB::DynDBDriver& dbdriver_;
  const std::vector<DB::DynDBDriver::Sensor_row> sensors_;
};

#endif // CANDIDATESELECTOR_H
