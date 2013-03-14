#include <algorithm>

#include "alignmentprocessor.h"

AlignmentProcessor::AlignmentProcessor(time_types::duration_t dt)
  : dt_(dt)
{
}

std::set<DetectionReport> AlignmentProcessor::getNextAlignedGroup()
{
  std::set<DetectionReport> result;
  std::set<DetectionReport>::iterator it = DRs_.begin();
  if (it == DRs_.end())
    return result;

  time_types::ptime_t lastTime = it->getSensorTime();
  result.insert(std::move(*it));
  it = DRs_.erase(it);
  while (it != DRs_.end())
  {
    if (it->getSensorTime() - lastTime <= dt_)
    {
      result.insert(std::move(*it));
      it = DRs_.erase(it);
    }
    else // because collection is sorted by time, when first time went out of bound,
      return result; // we can't find anything interesting further (sweeping algorithm)
  }
}

void AlignmentProcessor::setDRsCollection(const std::set<DetectionReport>& DRs)
{
  DRs_ = DRs;
  // set is used to speed up getNextAlignedGroup(),
  //  because it uses std::less<DetectionReport> to place DRs in proper order
}
