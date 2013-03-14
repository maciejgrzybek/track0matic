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
  typedef typename std::array<
                        typename StateModel::values_type,StateModel::Dimensions
                             > vector_t;
  virtual vector_t predict(vector_t u = vector_t()) = 0;
  virtual vector_t correct(vector_t z) = 0;
  virtual EstimationFilter* clone() const = 0;
};

template <class StateModel = PositionAndVelocityModel>
class KalmanFilter : public EstimationFilter<StateModel>
{
public:
  typedef typename EstimationFilter<StateModel>::vector_t vector_t;


  virtual vector_t predict(vector_t u = vector_t())
  {
    // TODO implement this
  }

  virtual vector_t correct(vector_t z)
  {
    // TODO implement this
  }

  virtual EstimationFilter<StateModel>* clone() const
  {
    return new KalmanFilter<StateModel>(); // FIXME add proper arguments for constructor
  }

private:
  /*
   vector_t stateVector_;
   ...
   */
};

} // namespace estimation

#endif // ESTIMATIONFILTER_H
