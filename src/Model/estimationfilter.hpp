#ifndef ESTIMATIONFILTER_H
#define ESTIMATIONFILTER_H

#include <array>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/vector.hpp>

namespace ublas = boost::numeric::ublas;

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

  virtual ~EstimationFilter()
  {}

  /**
   * @brief predicts (estimates) next state of modeled phenomenon
   * @param u - input (control) signal
   * @return pair of predicted state and prediction variance, as vectors
   */
  virtual std::pair<vector_t,vector_t> predict(const vector_t* u = nullptr) = 0;

  /**
   * @brief corrects prediction of modeled phenomenon state,
   *  basing on given measurements
   * @param z - measurements vector
   * @return pair of corrected state and it's variance, as vectors
   */
  virtual std::pair<vector_t,vector_t> correct(const vector_t& z) = 0;

  /**
   * @brief Initializes estimation filter with given vector of state
   *  and given variance for this state
   *
   * It's not pure virtual,
   *  because not every estimation filter would need initialization phase
   * @param state of model
   * @param parameter indicating how uncertain the state is
   */
  virtual std::pair<vector_t,vector_t>
    initialize(vector_t /*state*/, vector_t /*varianceError*/)
  {}

  virtual std::unique_ptr<EstimationFilter<StateModel> > clone() const = 0;
};

/**
 * Usage:
 *  KalmanFilter kf;
 *  kf.initializeState(state,covErr);
 *  kf.predict(); // returns predicted state
 *  kf.correct(measurement);
 *  kf.predict(); // and so on...
 */
template <class StateModel = PositionAndVelocityModel>
class KalmanFilter : public EstimationFilter<StateModel>
{
public:
  typedef typename EstimationFilter<StateModel>::vector_t vector_t;

  typedef ublas::vector<typename StateModel::values_type> Vector;
  typedef ublas::matrix<typename StateModel::values_type> Matrix;

  KalmanFilter(Matrix A, Matrix B, Matrix R, Matrix Q, Matrix H)
    : initialized(false),
      transitionModel(A),
      controlModel(B),
      measurementNoise(R),
      processNoise(Q),
      measurementModel(H)
  {}

  KalmanFilter(Vector ps, // predictedState
               Vector cs, // correctedState
               Matrix pce, // predictedCovarianceError
               Matrix cce, // correctedCovarianceError
               Matrix A, // transitionModel
               Matrix B, // controlModel
               Matrix R, // measurementNoise
               Matrix Q, // processNoise
               Matrix H) // measurementModel
    : initialized(true),
      predictedState(ps),
      correctedState(cs),
      predictedCovarianceError(pce),
      correctedCovarianceError(cce),
      transitionModel(A),
      controlModel(B),
      measurementNoise(R),
      processNoise(Q),
      measurementModel(H)
  {}

  virtual ~KalmanFilter()
  {}

  virtual std::pair<vector_t,vector_t> predict(const vector_t* u = nullptr)
  {
    assert(initialized);
    // predictedState (x')
    //  = transitionModel (A) * correctedState (x) + controlModel (B) * u
    predictedState = ublas::prod(transitionModel,correctedState);
    // predictedCovarianceError (P')
    //  = transitionModel (A) * correctedCovarianceError (P)
    //    * transposed (transitionModel (A)) + processNoise (Q)
    Matrix transposedTrModel = ublas::trans(transitionModel);
    Matrix mult = ublas::prod(correctedCovarianceError,transposedTrModel);
    predictedCovarianceError = ublas::prod(transitionModel,mult) + processNoise;

    if (u != nullptr)
      predictedState += ublas::prod(controlModel,arrayToUblas(*u,controlModel.size2()));

    return std::pair<vector_t,vector_t>(
            ublasToArray(predictedState),
            getDiagonal(predictedCovarianceError)
          );
  }

  virtual std::pair<vector_t,vector_t> correct(const vector_t& z)
  {
    assert(initialized);
    // Kalman gain = P' * transposed(H) / (H * P' * transposed(H) + R)

    Matrix transposedMeasurement = ublas::trans(measurementModel);
    Matrix top = ublas::prod(predictedCovarianceError,transposedMeasurement);
    // bottom = H * P' * transposed(H) + R
    Matrix bottom1 = ublas::prod(measurementModel, predictedCovarianceError);
    Matrix bottom_all = ublas::prod(bottom1,transposedMeasurement) + measurementNoise;
    Matrix bottom(bottom_all.size1(),bottom_all.size2());
    bool inverted = invertMatrix(bottom_all,bottom);
    assert(inverted);

    Matrix K = ublas::prod(top,bottom); // Kalman gain

    Vector residual
        = arrayToUblas(z,measurementModel.size1()) - ublas::prod(measurementModel,predictedState);

    // correctedState = predictedState + kalmanGain*(z - H*x')
    correctedState = predictedState + ublas::prod(K,residual);

    // P = (I - K*H)*P'
    Matrix KH = ublas::prod(K,measurementModel);
    Matrix Iminus
       = ublas::identity_matrix<typename Vector::value_type>(KH.size1(),KH.size2()) - KH;
    correctedCovarianceError = ublas::prod(Iminus,predictedCovarianceError);

    return std::pair<vector_t,vector_t>(
            ublasToArray(correctedState),
            getDiagonal(correctedCovarianceError)
          );
  }

  virtual std::pair<vector_t,vector_t>
    initialize(vector_t state, vector_t varianceError)
  {
    // translate whole vector_t to Vector
    Vector vec = arrayToUblas(state,std::tuple_size<vector_t>::value);
    Matrix m = diagonalVectorToMatrix(varianceError);

    return initializeState(vec,m);
  }

  virtual void setTransitionModel(Matrix m)
  {
    transitionModel = m;
  }

  virtual void setControlModel(Matrix v)
  {
    controlModel = v;
  }

  virtual void setMeasurementModel(Matrix m)
  {
    measurementModel = m;
  }

  virtual void setMeasurementNoise(Matrix m)
  {
    measurementNoise = m;
  }

  virtual void setProcessNoise(Matrix m)
  {
    processNoise = m;
  }

  virtual Vector getPredictedState() const
  {
    return predictedState;
  }

  virtual Vector getCorrectedState() const
  {
    return correctedState;
  }

  virtual Matrix getPredictedCovErr() const
  {
    return predictedCovarianceError;
  }

  virtual Matrix getCorrectedCovErr() const
  {
    return correctedCovarianceError;
  }

  virtual std::unique_ptr<EstimationFilter<StateModel> > clone() const
  {
    std::unique_ptr<EstimationFilter<StateModel> > result(
          new KalmanFilter<StateModel>(*this));
    return result;
  }

private:
  // only after successful end of this method execution, filter is properly initialized
  virtual std::pair<vector_t,vector_t>
    initializeState(Vector state, Matrix covarianceError)
  {
    correctedState = state;
    correctedCovarianceError = covarianceError;
    initialized = true;
    return predict(); // needed to setup predictedCovarianceError
  }

  // copies elements from Vector to vector_t collection.
  vector_t ublasToArray(const Vector& vec)
  {
    vector_t result;
    std::size_t i = 0;
    for (typename StateModel::values_type v : vec)
    {
      result[i] = v;
      ++i;
    }

    return result;
  }

  // copies no first elements from vector_t (array) to Vector (ublas vector)
  Vector arrayToUblas(const vector_t& vec, std::size_t no)
  {
    no = std::min(no,vec.size()); // to ensure not steping over the range
    Vector result(no);
    for (std::size_t i = 0; i<no; ++i)
    {
      result(i) = vec[i];
    }

    return result;
  }

  // returns diagonal (as a vector) of a given matrix
  vector_t getDiagonal(const Matrix& m)
  {
    assert(m.size1() == m.size2() &&
           m.size1() == std::tuple_size<vector_t>::value);
    vector_t result;
    for (unsigned i = 0; i < m.size1(); ++i)
    {
      result[i] = m(i,i);
    }

    return result;
  }

  // returns matrix with items from vector on it's diagonal
  Matrix diagonalVectorToMatrix(const vector_t& v)
  {
    std::size_t size = std::tuple_size<vector_t>::value;
    Matrix result(size,size);
    result.clear();
    for (unsigned int i = 0; i < size; ++i)
    {
      result(i,i) = v[i];
    }

    return result;
  }

  /*
   * Matrix inversion routine.
   *   Uses lu_factorize and lu_substitute in uBLAS to invert a matrix
   *
   *  found at: http://savingyoutime.wordpress.com/2009/09/21/c-matrix-inversion-boostublas/
   */
  template<class T>
  bool invertMatrix(const ublas::matrix<T>& input, ublas::matrix<T>& inverse)
  {
    typedef ublas::permutation_matrix<std::size_t> pmatrix;

    // create a working copy of the input
    ublas::matrix<T> A(input);

    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());

    // perform LU-factorization
    int res = ublas::lu_factorize(A, pm);
    if (res != 0)
      return false;

    // create identity matrix of "inverse"
    inverse.assign(ublas::identity_matrix<T> (A.size1()));

    // backsubstitute to get the inverse
    ublas::lu_substitute(A, pm, inverse);

    return true;
  }

  bool initialized;

  Vector predictedState; // X'(k) (a priori)
  Vector correctedState; // X(k) (a posteriori)

  Matrix predictedCovarianceError; // P'(k) (a priori)
  Matrix correctedCovarianceError; // P(k) (a posteriori)

  Matrix transitionModel; // A
  Matrix controlModel; // B

  Matrix measurementNoise; // R
  Matrix processNoise; // Q

  Matrix measurementModel; // H
};

} // namespace estimation

#endif // ESTIMATIONFILTER_H
