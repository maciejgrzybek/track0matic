#include <vector>

#include "candidateselector.h"

CandidateSelector::CandidateSelector(DB::DynDBDriver& dbdriver)
  : dbdriver_(dbdriver),
    sensors_(dbdriver.getSensors())
{
}


std::vector<std::vector<DetectionReport> >
CandidateSelector::getMeasurementGroups(const std::vector<DetectionReport>&) const
{
  // TODO implement this
}
