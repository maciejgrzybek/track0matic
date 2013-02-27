#include <stdexcept>

#include "resultcomparator.h"

ResultComparator::ResultComparator(const feature_grade_map_t& gradeRates)
  : gradeRates_(gradeRates)
{
}

AndComparator::AndComparator(const feature_grade_map_t& gradeRates)
  : ResultComparator(gradeRates)
{}

double AndComparator::operator()(const feature_grade_map_t& featureGrades)
{
  return evaluateGrades(featureGrades);
}

double AndComparator::evaluateGrades(const feature_grade_map_t& featureGrades)
{
  double result = 1;
  for (auto& feature : featureGrades)
  {
    feature_grade_map_t::mapped_type gradeRate = 0;
    try
    {
      gradeRate = gradeRates_.at(feature.first);
    }
    catch (const std::out_of_range&)
    {
      // nothing do to here, gradeRate already set to 0, on initialization
    }
    result *= (gradeRate*feature.second);
  }
  return result;
}


OrComparator::OrComparator(const feature_grade_map_t& gradeRates)
  : ResultComparator(gradeRates)
{}

double OrComparator::operator()(const feature_grade_map_t& featureGrades)
{
  return evaluateGrades(featureGrades);
}

double OrComparator::evaluateGrades(const feature_grade_map_t& featureGrades)
{
  int i = 0;
  double result = 0;
  for (auto& feature : featureGrades)
  {
    feature_grade_map_t::mapped_type gradeRate = 0;
    try
    {
      gradeRate = gradeRates_.at(feature.first);
    }
    catch (const std::out_of_range&)
    {
      // nothing do to here, gradeRate already set to 0, on initialization
    }
    result += (gradeRate*feature.second);
    ++i;
  }
  return result/i; // normalize to number of features
}
