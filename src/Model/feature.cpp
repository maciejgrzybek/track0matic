#include "feature.h"
#include "featureextractor.h"

Feature::Feature()
{
}

void ColorFeature::accept(FeatureVisitor& v) const
{
  v.visit(*this);
}

void PlateFeature::accept(FeatureVisitor& v) const
{
  v.visit(*this);
}
