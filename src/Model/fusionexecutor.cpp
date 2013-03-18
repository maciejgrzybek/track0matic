#include "fusionexecutor.h"

void FusionExecutor::fuseDRs(
    std::map<std::shared_ptr<Track>,std::set<DetectionReport> >& collection)
{
   for (auto& item : collection)
   {
     std::shared_ptr<Track> track = item.first;
     for (const DetectionReport& DR : item.second)
     {
       track->applyMeasurement(DR);
     }
   }

}
