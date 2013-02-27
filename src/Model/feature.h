#ifndef FEATURE_H
#define FEATURE_H

#include <string>

class FeatureVisitor;

class Feature
{
public:
  Feature();
  virtual void accept(FeatureVisitor&) const = 0;
  virtual std::string getName() const = 0;
};

class ColorFeature : public Feature
{
public:
  virtual void accept(FeatureVisitor&) const;
  virtual std::string getName() const;
};

class PlateFeature : public Feature
{
public:
  virtual void accept(FeatureVisitor&) const;
  virtual std::string getName() const;
};

#endif // FEATURE_H
