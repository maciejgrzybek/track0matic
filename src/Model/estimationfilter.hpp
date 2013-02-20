#ifndef ESTIMATIONFILTER_H
#define ESTIMATIONFILTER_H

#include <array>

namespace estimation
{

struct PositionAndVelocityModel
{
  typedef double values_type;
  enum { Dimensions = 4 };
};

template <class StateModel = PositionAndVelocityModel>
class EstimationFilter
{
public:
  virtual std::array<typename StateModel::values_type,StateModel::Dimensions>
    predict(double u = 0) = 0;
  virtual std::array<typename StateModel::values_type,StateModel::Dimensions>
    correct(double z) = 0;
};

template <class StateModel = PositionAndVelocityModel>
class KalmanFilter : public EstimationFilter<StateModel>
{
  // TODO implement this
};

} // namespace estimation

#endif // ESTIMATIONFILTER_H
