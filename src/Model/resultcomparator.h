#ifndef RESULTCOMPARATOR_H
#define RESULTCOMPARATOR_H

#include <map>
#include <string>

//TODO implement this
class ResultComparator
{
public:
  typedef std::map<std::string,double> feature_grade_map_t;

  virtual double operator()(const feature_grade_map_t&) = 0;

protected:
  ResultComparator(const feature_grade_map_t& gradeRates);
  feature_grade_map_t gradeRates_;
};

class AndComparator : public ResultComparator
{
public:
  AndComparator(const feature_grade_map_t& gradeRates);
  virtual double operator()(const feature_grade_map_t&);

private:
  double evaluateGrades(const feature_grade_map_t& featureGrades);
};

class OrComparator : public ResultComparator
{
public:
  OrComparator(const feature_grade_map_t& gradeRates);
  virtual double operator()(const feature_grade_map_t&);

private:
  double evaluateGrades(const feature_grade_map_t& featureGrades);
};

// TODO implement this
class ListResultComparator
{
};

#endif // RESULTCOMPARATOR_H
