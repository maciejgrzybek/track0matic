#ifndef CANDIDATESELECTOR_H
#define CANDIDATESELECTOR_H

#include <memory>
#include <set>
#include <vector>

#include "sensor.h"
#include "detectionreport.h"
#include "DB/dyndbdriver.h"

// TODO do not group DRs from sensors from neighborhood (basis only on sensors neighborhood),
//  instead of this, try to figure out which DR could be seen by (more) sensors and group these.
// It's important because in some cases, there could be sensors qualified to be in neighborhood
// (e.g. because of partial common coverage of view), but in fact DR could not be seen by 2 sensors from this kind of neighborhood.
// In such a case, it's important to check DRs each other for neighborhood, according to their sensors details.
// Warning! Consider complexity of such operation, maybe it's not worth spending so much time calculating "improvemed" version, which gives almost zero impact on final result
// IMPORTANT! This class has to provide uniqueness of assignment DRs to groups, to avoid clonning DRs!
// The best solution is probably to choose this group for DR, which has more sensors.
class CandidateSelector
{
public:
  /**
   * @brief c-tor loads sensors descriptions from given set.
   * @param Reference to set of Sensor pointers.
   */
  CandidateSelector(const std::set<Sensor*>& sensorsSet);

  /**
   * @brief c-tor loads sensors descriptions (from DynDBDriver).
   * @param Shared ptr to Dynamic DB Driver, which will be used to get sensors descriptions
   */
  CandidateSelector(std::shared_ptr<DB::DynDBDriver> dbdriver);

  /**
   * @brief Method groups DRs by characteristics of sensor,
   *  like it's range and location.
   *
   *  Each returned element contains collection of DRs from sensors from neighborhood.
   *  To determine neighborhood, range and location of sensors are used.
   *  Each DR
   * @return collection of collection of DRs
   */
  std::vector<std::set<DetectionReport> >
    getMeasurementGroups(const std::set<DetectionReport> &) const;

private:
  std::shared_ptr<const DB::DynDBDriver> dbdriver_;
  const std::set<Sensor*> sensors_;
};

#endif // CANDIDATESELECTOR_H
