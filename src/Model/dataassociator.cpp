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

std::set<DetectionReport> DataAssociator::getListForTrack(const Track&)
{
  // TODO implement this
}

std::pair<double,std::set<DetectionReport> >
DataAssociator::rateListForTrack(std::set<DetectionReport>&,const Track&) const
{
  // TODO implement this
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
