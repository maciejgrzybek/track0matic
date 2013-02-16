#include <vector>

#include "candidateselector.h"

CandidateSelector::CandidateSelector(DB::DynDBDriver& dbdriver)
  : dbdriver_(dbdriver),
    sensors_(dbdriver.getSensors())
{
}


std::vector<std::set<DetectionReport> >
CandidateSelector::getMeasurementGroups(const std::set<DetectionReport>&) const
{
  // TODO implement this
}
