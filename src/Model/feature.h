#ifndef FEATURE_H
#define FEATURE_H

class FeatureVisitor;

class Feature
{
public:
  Feature();
  virtual void accept(FeatureVisitor&) const = 0;
};

class ColorFeature : public Feature
{
public:
  virtual void accept(FeatureVisitor&) const;
};

class PlateFeature : public Feature
{
public:
  virtual void accept(FeatureVisitor&) const;
};

#endif // FEATURE_H
