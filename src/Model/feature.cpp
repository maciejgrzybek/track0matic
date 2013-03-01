#include "feature.h"
#include "featureextractor.h"

void ColorFeature::accept(FeatureVisitor& v) const
{
  v.visit(*this);
}

std::string ColorFeature::getName() const
{
  return "Color";
}

Feature* ColorFeature::clone() const
{
  return new ColorFeature(*this);
}

void PlateFeature::accept(FeatureVisitor& v) const
{
  v.visit(*this);
}

std::string PlateFeature::getName() const
{
  return "Plate";
}

Feature* PlateFeature::clone() const
{
  return new PlateFeature(*this);
}
