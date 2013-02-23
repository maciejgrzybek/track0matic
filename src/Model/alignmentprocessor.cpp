#include <algorithm>

#include "alignmentprocessor.h"

AlignmentProcessor::AlignmentProcessor(boost::posix_time::time_duration dt)
  : dt_(dt)
{
}

std::vector<DetectionReport> AlignmentProcessor::getNextAlignedGroup()
{
  std::vector<DetectionReport> result;
  std::vector<DetectionReport>::iterator it = DRs_.begin();
  if (it == DRs_.end())
    return result;

  boost::posix_time::ptime lastTime = it->getSensorTime();
  result.push_back(std::move(*it));
  it = DRs_.erase(it);
  while (it != DRs_.end())
  {
    if (it->getSensorTime() - lastTime <= dt_)
    {
      result.push_back(std::move(*it));
      it = DRs_.erase(it);
    }
    else // because collection is sorted by time, when first time went out of bound,
      return result; // we can't find anything interesting further (sweeping algorithm)
  }
}

void AlignmentProcessor::setDRsCollection(const std::vector<DetectionReport>& DRs)
{
  DRs_ = DRs;
  std::sort(DRs_.begin(),DRs_.end(),std::less<DetectionReport>()); // sort to speed up getNextAlignedGroup()
}
