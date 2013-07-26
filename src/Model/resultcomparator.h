#ifndef RESULTCOMPARATOR_H
#define RESULTCOMPARATOR_H

#include <map>
#include <string>
#include <vector>

class DetectionReport;
class Track;

class ResultComparator
{
public:
  typedef std::map<std::string,double> feature_grade_map_t;

  virtual double operator()(const feature_grade_map_t&,
                            const DetectionReport&,
                            const Track&) = 0;

protected:
  ResultComparator(const feature_grade_map_t& gradeRates);
  feature_grade_map_t gradeRates_;
};

class AndComparator : public ResultComparator
{
public:
  AndComparator(const feature_grade_map_t& gradeRates);
  virtual double operator()(const feature_grade_map_t&,
                            const DetectionReport&,
                            const Track&);

private:
  double evaluateGrades(const feature_grade_map_t& featureGrades,
                        const DetectionReport& dr,
                        const Track& t) const;
};

class OrComparator : public ResultComparator
{
public:
  OrComparator(const feature_grade_map_t& gradeRates);
  virtual double operator()(const feature_grade_map_t&,
                            const DetectionReport&,
                            const Track&);

private:
  double evaluateGrades(const feature_grade_map_t& featureGrades,
                        const DetectionReport& dr,
                        const Track& t) const;
};

class ListResultComparator
{
public:
  typedef std::vector<double> rates_collection_t;

  virtual double operator()(const rates_collection_t&) = 0;

protected:
  ListResultComparator() = default;
};

class AndListComparator : public ListResultComparator
{
public:
  virtual double operator()(const rates_collection_t&);

private:
  double evaluateRates(const rates_collection_t&) const;
};

class OrListComparator : public ListResultComparator
{
public:
  virtual double operator()(const rates_collection_t&);

private:
  double evaluateRates(const rates_collection_t&) const;
};

#endif // RESULTCOMPARATOR_H
