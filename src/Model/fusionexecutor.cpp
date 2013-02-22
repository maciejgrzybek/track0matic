#include "fusionexecutor.h"

FusionExecutor::FusionExecutor()
{
}

void FusionExecutor::fuseDRs(
    std::map<std::shared_ptr<Track>,std::set<DetectionReport> >& collection)
{
  //TODO implement this, according to below pseudocode
  /*
   for (auto& item : collection)
   {
     for (auto& DR : item.second)
     {
       item.first->estimationFilter.correct(DR);
     }
   }
   */

}
