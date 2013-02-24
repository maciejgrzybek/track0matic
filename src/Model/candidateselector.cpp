#include <vector>

#include "candidateselector.h"

CandidateSelector::CandidateSelector(const std::set<Sensor*>& sensorsSet)
  : dbdriver_(NULL),
    sensors_(sensorsSet)
{
}

CandidateSelector::CandidateSelector(DB::DynDBDriver& dbdriver)
  : dbdriver_(&dbdriver),
    sensors_(dbdriver.getSensors())
{
}


std::vector<std::set<DetectionReport> >
CandidateSelector::getMeasurementGroups(const std::set<DetectionReport>& DRs) const
{
  // TODO improve this!
  // simple implementation which puts all DRs into the same group
  // can be errorneous, because e.g. distant DRs could be associated for one Track.
  std::vector<std::set<DetectionReport> > result;
  result.push_back(DRs);
  return result;
}
