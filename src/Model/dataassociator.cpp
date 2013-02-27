#include <utility>

#include "dataassociator.h"

void DataAssociator::compute()
{
  /*
   foreach Track
   {
    DRs = getListForTrack(Track); // after this line, DRGroups are thiner of DRs (chosen ones)
    associatedTracks.append(DRs);
    //notAssociatedTracks are built by getListForTrack method.
   }
   */
}

std::map<std::shared_ptr<Track>,std::set<DetectionReport> > DataAssociator::getDRsForTracks()
{
  // TODO implement this
}

std::set<DetectionReport> DataAssociator::getNotAssociated()
{
  // TODO implement this
}

void DataAssociator::setInput(std::vector<std::set<DetectionReport> > DRsGroups)
{
  DRGroups = std::move(DRsGroups);
}

void DataAssociator::setDRResultComparator(std::unique_ptr<ResultComparator> comparator)
{
  resultComparator = std::move(comparator);
}

void DataAssociator::setListResultComparator(std::unique_ptr<ListResultComparator> comparator)
{
  listResultComparator = std::move(comparator);
}

void DataAssociator::setFeatureExtractor(std::unique_ptr<FeatureExtractor> extractor)
{
  featureExtractor = std::move(extractor);
}

void DataAssociator::setDRRateThreshold(double threshold)
{
  DRRateThreshold = threshold;
}

std::set<DetectionReport> DataAssociator::getListForTrack(const Track&)
{
  // TODO implement this
}

std::pair<double,std::set<DetectionReport> >
DataAssociator::rateListForTrack(std::set<DetectionReport>& DRs,const Track& track) const
{
  std::vector<double> rates;
  std::set<DetectionReport> result;
  auto it = DRs.begin();
  auto endIt = DRs.end();
  while (it != endIt)
  {
    double DRRate = rateDRForTrack(*it, track);
    if (DRRate >= DRRateThreshold)
    {
      // take item from DRs set and put into result collection
      result.insert(*it);
      rates.push_back(DRRate);

      it = DRs.erase(it);
    }
  }
  // here, all choosen detection reports were taken out of DRs collection
  // there are only not matching left.
  double overallRate = listResultComparator->operator()(rates);
  return std::pair<double,std::set<DetectionReport> >(overallRate,result);
}

double DataAssociator::rateDRForTrack(const DetectionReport& dr, const Track& track) const
{
  ResultComparator::feature_grade_map_t m;
  for (Feature* drFeature : dr.getFeatures())
  {
    std::string name = drFeature->getName();
    Track::features_set_t trackFeatures = track.getFeatures();
    m[name] = 0;

    // TODO can be optimized - linear search vs logarithmic
    for (Feature* feature : trackFeatures)
    {
      if (feature->getName() == name)
      {
        double result = featureExtractor->compare(*drFeature,*feature);
        m[name] = result; // we assume, that there are only one Feature with given name in set
      }
    }
  }

  return resultComparator->operator()(m);
}
