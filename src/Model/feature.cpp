#include "feature.h"
#include "featureextractor.h"

Feature::Feature()
{
}

void ColorFeature::accept(FeatureVisitor& v) const
{
  v.visit(*this);
}

std::string ColorFeature::getName() const
{
  return "Color";
}

void PlateFeature::accept(FeatureVisitor& v) const
{
  v.visit(*this);
}

std::string PlateFeature::getName() const
{
  return "Plate";
}
