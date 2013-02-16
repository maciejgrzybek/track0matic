#include "alignmentprocessor.h"

AlignmentProcessor::AlignmentProcessor(boost::posix_time::time_period dt)
  : dt_(dt)
{
}

std::vector<DetectionReport> AlignmentProcessor::getNextAlignedGroup() const
{
  // TODO implement this
}

void AlignmentProcessor::setDRsCollection(const std::vector<DetectionReport>& DRs)
{
  DRs_ = DRs;
}
