#include "fusionexecutor.h"

void FusionExecutor::fuseDRs(
    std::map<std::shared_ptr<Track>,std::set<DetectionReport> >& collection)
{
   for (auto& item : collection)
   {
     std::shared_ptr<Track> track = item.first;
     estimation::EstimationFilter<>& estimationFilter
         = track->getEstimationFilter();
     for (const DetectionReport& DR : item.second)
     {
       estimation::EstimationFilter<>::vector_t vec = DRToVector(track,DR);
       estimationFilter.correct(vec);
     }
   }

}

estimation::EstimationFilter<>::vector_t
  FusionExecutor::DRToVector(std::shared_ptr<Track> track,
                             const DetectionReport& DR)
{
  estimation::EstimationFilter<>::vector_t result;
  // valid only for vector size of 4
  // if vector_t size changes, reimplement this method
  static_assert(result.max_size() == 4,
      "Method implemented only for fixed size of vector");

  result[0] = DR.getLongitude();
  result[1] = DR.getLatitude();
  result[2] = 0; // TODO read speed of object using track->getVelocity() method
  result[3] = 0;

  return result;
}
