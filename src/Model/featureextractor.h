#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H

#include "feature.h"

class FeatureVisitor
{
public:
  virtual void visit(const ColorFeature&) = 0;
  virtual void visit(const PlateFeature&) = 0;
};

/**
 * @brief The FeatureExtractor class, uses multimethod design pattern (based on visitor design pattern)
 *  It's task is to compare and fuse features, when it's possible.
 *  You may define many types of available fusions and comparations, without changing interface of this class.
 *  E.g. fusion of two colors, can be realized as a linear combination of them.
 */
class FeatureExtractor
{
public:
  virtual ~FeatureExtractor();

  double compare(const Feature& a, const Feature& b) const;
  Feature* fuse(const Feature& a, const Feature& b) const;

  // creates ColorVisitor => second feature is ColorFeature
  virtual void visit(const ColorFeature&);
  // creates PlateVisitor => second feature is PlateFeature
  virtual void visit(const PlateFeature&);

protected:
  /*
   * Implementations of concrete comparators.
   * Can be overloaded to provide other comparators for color and plate features.
   */
  virtual double compare(const ColorFeature& a, const ColorFeature& b) const;
  virtual double compare(const PlateFeature& a, const PlateFeature& b) const;
  //comparing Plate with Color does not make sense for the moment, but if possible - implement applicable comparator

  /*
   * Implementation of concrete fusioners;
   * Can be overloaded to provide different type of color fusion.
   */
  virtual Feature* fuse(const ColorFeature& a, const ColorFeature& b) const;
  //Feature fuse(const PlateFeature& a, const PlateFeature& b) const;
  //fusing Plate with Color does not make sense for the moment, but if possible in future, implement applicable fuser

private:
  class ColorVisitor : public FeatureVisitor
  {
  public:
    virtual void visit(const ColorFeature&);
    virtual void visit(const PlateFeature&);

    // value for compare(Color,X)
    double value;
    // feature for fuse(Color,X)
    Feature* fValue;
  };

  class PlateVisitor : public FeatureVisitor
  {
  public:
    virtual void visit(const ColorFeature&);
    virtual void visit(const PlateFeature&);

    double value;
    Feature* fValue;
  };

};

#endif // FEATUREEXTRACTOR_H
