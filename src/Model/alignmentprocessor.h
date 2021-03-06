#ifndef ALIGNMENTPROCESSOR_H
#define ALIGNMENTPROCESSOR_H

#include <set>

#include <Common/time.h>
#include "detectionreport.h"

namespace exceptions
{
  class NoDRsAvailable : public std::exception
  {
   public:
    virtual ~NoDRsAvailable() throw()
    {}

    virtual const char* what() const throw()
    {
      return "No detection reports available.";
    }
  };
} // namespace exceptions

class AlignmentProcessor
{
public:
  /**
   * @brief c-tor, gets time delta, which is used to group DRs together
   *  DRs which timestamps diff is lower than dt are taken into account,
   *  as taken from the same time period.
   * @param delta time
   */
  AlignmentProcessor(time_types::duration_t dt);

  /**
   * @brief Gives next aligned group from DRs_ collection,
   *  each pair of DRs in result group,
   *  has to has time difference lower than dt_
   *
   *  Each aligned group is ordered by time
   *  (earlier DRs in group have not higher timestamps (=<) - ascending order)
   *  After returning group, further invokations of this method
   *  results in returning next aligned groups from the same set of DRs.
   *  Following groups have not lower timestamps than previous (=<) - ascending order.
   *  This method removes DRs from DRs_ collection (it takes items from collection)!
   *  After invoking getNextAlignedGroup() when DRs_ collection is empty,
   *  exception exceptions::NoDRsAvailable is thrown
   *
   * @return collection of DRs with time interval not higher than dt_
   * @throws exceptions::NoDRsAvailable
   */
  std::set<DetectionReport> getNextAlignedGroup();

  /**
   * @brief Copies given collection of DRs, for further manipulation by AlignmentProcessor
   * @param reference to vector of DRs, which will be copied.
   */
  void setDRsCollection(const std::set<DetectionReport>&);

private:
  std::set<DetectionReport> DRs_;
  const time_types::duration_t dt_;
};

#endif // ALIGNMENTPROCESSOR_H
