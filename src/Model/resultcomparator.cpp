#include <cmath>
#include <stdexcept>

#include <Common/configurationmanager.h>

#include "detectionreport.h"
#include "track.h"

#include "resultcomparator.h"

ResultComparator::ResultComparator(const feature_grade_map_t& gradeRates)
  : gradeRates_(gradeRates)
{}

AndComparator::AndComparator(const feature_grade_map_t& gradeRates)
  : ResultComparator(gradeRates)
{}

double AndComparator::operator()(const feature_grade_map_t& featureGrades,
                                 const DetectionReport& dr,
                                 const Track& t)
{
  return evaluateGrades(featureGrades,dr,t);
}

double AndComparator::evaluateGrades(const feature_grade_map_t& featureGrades,
                                     const DetectionReport& dr,
                                     const Track& t) const
{
  double featuresResult = 1;
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
    featuresResult *= (gradeRate*feature.second);
  }


  double positionResult = 1/sqrt(
          pow((dr.getLongitude() - t.getLongitude()),2)
          +
          pow((dr.getLatitude() - t.getLatitude()),2)
          +
          pow((dr.getMetersOverSea() - t.getMetersOverSea()),2)
        );

    double maximumPositionRate
        = Common::Configuration::ConfigurationManager
            ::getCastedValue<double>("Model",
                                     "ResultComparator.MaximumPositionRate",
                                     100000000);

  if (positionResult > maximumPositionRate)
    positionResult = maximumPositionRate;

  return featuresResult * positionResult/maximumPositionRate;
}


OrComparator::OrComparator(const feature_grade_map_t& gradeRates)
  : ResultComparator(gradeRates)
{}

double OrComparator::operator()(const feature_grade_map_t& featureGrades,
                                const DetectionReport& dr,
                                const Track& t)
{
  return evaluateGrades(featureGrades,dr,t);
}

double OrComparator::evaluateGrades(const feature_grade_map_t& featureGrades,
                                    const DetectionReport& dr,
                                    const Track& t) const
{
  int i = 0;
  double featuresResult = 0;
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
    featuresResult += (gradeRate*feature.second);
    ++i;
  }

  double positionResult = 1/sqrt(
          pow((dr.getLongitude() - t.getLongitude()),2)
          +
          pow((dr.getLatitude() - t.getLatitude()),2)
          +
          pow((dr.getMetersOverSea() - t.getMetersOverSea()),2)
        );

    double maximumPositionRate
        = Common::Configuration::ConfigurationManager
            ::getCastedValue<double>("Model",
                                     "ResultComparator.MaximumPositionRate",
                                     100000000);

  if (positionResult > maximumPositionRate)
    positionResult = maximumPositionRate;

  double positionNormalized = positionResult/maximumPositionRate;

  double featuresNormalized = 0; // rate normalized to number of features
  if (i > 0)
  {
    featuresNormalized = featuresResult/i;
    return (featuresNormalized
            + positionNormalized)/2; // take into consideration difference in position
  }
  else // if no features given
    return positionNormalized; // only position is included
}

double AndListComparator::operator()(const rates_collection_t& c)
{
  return evaluateRates(c);
}

double AndListComparator::evaluateRates(const rates_collection_t& c) const
{
  double result = 1;
  for (auto i : c)
  {
    result *= i;
  }

  return result;
}

double OrListComparator::operator()(const rates_collection_t& c)
{
  return evaluateRates(c);
}

double OrListComparator::evaluateRates(const rates_collection_t& c) const
{
  double result = 0;
  int cnt = 0;
  for (auto i : c)
  {
    result += i;
    ++cnt;
  }

  if (cnt == 0)
    return 0;

  return result/cnt; // normalize to number of rates
}
