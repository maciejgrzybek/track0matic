#include "featureextractor.h"

FeatureExtractor::~FeatureExtractor()
{
}

void FeatureExtractor::visit(const ColorFeature&)
{
  // TODO implement this
}

void FeatureExtractor::visit(const PlateFeature&)
{
  // TODO implement this
}

double FeatureExtractor::compare(const ColorFeature& a, const ColorFeature& b) const
{
  // TODO implement this
}

double FeatureExtractor::compare(const PlateFeature& a, const PlateFeature& b) const
{
  // TODO implement this
}

Feature* FeatureExtractor::fuse(const ColorFeature& a, const ColorFeature& b) const
{
  // TODO implement this
}
