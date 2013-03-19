#include <algorithm>

#include "common/time.h"
#include "trackmanager.h"

TrackManager::TrackManager(double initializationThreshold)
  : initializationDRThreshold_(initializationThreshold)
{
}

std::map<std::shared_ptr<Track>,std::set<DetectionReport> >
  TrackManager::initializeTracks(const std::vector<std::set<DetectionReport> >& DRsGroups,
                                 std::unique_ptr<estimation::EstimationFilter<> > filter)
{
  // for each set from DRsGroups, invoke initializeTrack() and add returned Track to tracks_ set.
  // All initialized Tracks are collected and returned as map Track->DRs.  
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> > result;
  for (auto DRs : DRsGroups) // copies group, to allow modification
  { // for each DR set
    auto rated = getRatedPairs(DRs);
    std::vector<std::set<DetectionReport> > groups = chooseFromRated(rated);

    for (auto group : groups)
    {
      std::shared_ptr<Track> track
          = initializeTrack(group,filter->clone());

      tracks_.insert(track);
      result[track] = group;
    }
  }

  return result;
}

const std::set<std::shared_ptr<Track> >& TrackManager::getTracksRef() const
{
  return tracks_;
}

std::set<std::shared_ptr<Track> > TrackManager::getTracks() const
{
  return tracks_;
}

void TrackManager::setFeatureExtractor(std::unique_ptr<FeatureExtractor> extractor)
{
  featureExtractor_ = std::move(extractor);
}

std::shared_ptr<Track>
  TrackManager::initializeTrack(const std::set<DetectionReport>& DRs,
                                std::unique_ptr<estimation::EstimationFilter<> > filter)
{
  // TODO implement merging features from DRs for Track (based on FeatureExtractor's fuse() method).

  double lons = 0;
  double lats = 0;
  double moses = 0;
  std::size_t cnt = 0;
  time_types::ptime_t maxTime;
  for (const DetectionReport& DR : DRs)
  {
    lons += DR.getLongitude();
    lats += DR.getLatitude();
    moses += DR.getMetersOverSea();
    auto DRTime = DR.getSensorTime();
    if (DRTime > maxTime)
      maxTime = DRTime;
    ++cnt;
  }

  double lon = lons/cnt;
  double lat = lats/cnt;
  double mos = moses/cnt;

  std::shared_ptr<Track> track(
          new Track(filter->clone(),
                    lon,
                    lat,
                    mos,
                    maxTime)
        );

  return track;
}

std::pair<
TrackManager::DR_pair_rates_t, // rates
std::set<DetectionReport> // notAssigned
>
TrackManager::getRatedPairs(const std::set<DetectionReport>& DRs) const
{
  std::set<DetectionReport> notAssigned;
  std::set<DetectionReport>::const_iterator it = DRs.begin();
  std::set<DetectionReport>::const_iterator endIt = DRs.end();

  DR_pair_rates_t rates;

  std::set<DetectionReport> used;

  for (;it != endIt; ++it)
  { // for each DR
    auto nextIt = it;
    ++nextIt; // start from next

    for (; nextIt != endIt; ++nextIt)
    {
      double rate = compare(*it,*nextIt);
      if (rate >= initializationDRThreshold_)
      { // add every DR which is enough similar to current (it) DR
        std::tuple<DetectionReport,DetectionReport,double> t(*it,*nextIt,rate);
        rates.insert(t);

        used.insert(*it); // nothing bad happens, if already exists [TODO optimize this?]
        used.insert(*nextIt);
      }
    }
  }

  std::set_difference(DRs.begin(),DRs.end(),
                      used.begin(),used.end(),
                      std::inserter(notAssigned,notAssigned.end()),
                      std::less<DetectionReport>());

  return std::pair<
      DR_pair_rates_t,
      std::set<DetectionReport>
      >(rates,notAssigned);
}

std::vector<std::set<DetectionReport> >
TrackManager::chooseFromRated(std::pair<
                                        DR_pair_rates_t, // rates
                                        std::set<DetectionReport> // notAssigned
                                       > rated) const
{
  // returns vector of sets of DRs which are candidates for Track
  // each set from vector indices one Track to initialize

  std::vector<std::set<DetectionReport> > result;
  DR_pair_rates_t& tuples = rated.first;
  std::set<DetectionReport> tuplesOriginally = getDRsFromTuples(tuples);

  std::set<DetectionReport>& notAssigned = rated.second;
  std::set<DetectionReport> used;

  while (!tuples.empty())
  { // iterate over tuples of DR,DR,rate
    auto it = tuples.begin();
    std::set<DetectionReport> currentSet = { std::get<0>(*it),
                                             std::get<1>(*it) };
    used.insert(std::get<0>(*it));
    used.insert(std::get<1>(*it));

    // firstSet contains these DRs which are paired with first DR in tuple
    std::set<DetectionReport> firstSet
        = getSetOfPairedDRs(std::get<0>(*it),it,tuples.end());
    // secondSet contains these DRs which are paired with second DR in tuple
    std::set<DetectionReport> secondSet
        = getSetOfPairedDRs(std::get<1>(*it),it,tuples.end());

    auto i = firstSet.begin();
    while (i != firstSet.end())
    { // for each item from first set, try to find the same in second set
      if (secondSet.empty()) // if there is nothing more in second set,
          break; // we are sure, there is nothing to add to current set.
      auto foundIt = secondSet.find(*i);
      if (foundIt != secondSet.end())
      {
        currentSet.insert(*foundIt);
        used.insert(*foundIt);

        // sweeping
        secondSet.erase(foundIt);
      }
      i = firstSet.erase(i); // erase anyway, because if not found in second set,
                      // cannot be assigned to anything else, because one DR can be only in one set.
    }

    removeDRsFromTupleCollection(currentSet,tuples);
    result.push_back(currentSet);
  }

  // evaluates difference between all DRs from tuples collection at the beginning
  // and all DRs put into result
  // the result is appended to given notAssociated DRs (from parameter).
  std::set_difference(tuplesOriginally.begin(),tuplesOriginally.end(),
                      used.begin(),used.end(),
                      std::inserter(notAssigned,notAssigned.end()),
                      std::less<DetectionReport>());

  // push each single, not assigned DR, into result vector, in separate set
  for (const DetectionReport& dr : notAssigned)
  {
    std::set<DetectionReport> s = { dr };
    result.push_back(s);
  }

  return result;
}

std::set<DetectionReport> TrackManager::getDRsFromTuples(const DR_pair_rates_t& tuples) const
{
  std::set<DetectionReport> result;
  for (auto& tuple : tuples)
  {
    result.insert(std::get<0>(tuple)); //there won't be duplications because it's std::set
    result.insert(std::get<1>(tuple));
  }

  return result;
}

std::set<DetectionReport>
TrackManager::getSetOfPairedDRs(const DetectionReport& DR,
                                DR_pair_rates_t::const_iterator begin,
                                DR_pair_rates_t::const_iterator end) const
{
  std::set<DetectionReport> result;
  for (; begin != end; ++begin)
  {
    if (std::get<0>(*begin) == DR)
    { // if first element of pair (DR,DR) is equal given DR
      result.insert(std::get<1>(*begin)); // return second
    }
    else if (std::get<1>(*begin) == DR)
    { // analogously to above
      result.insert(std::get<0>(*begin));
    }
  }

  return result;
}

std::size_t
TrackManager::removeDRsFromTupleCollection(const std::set<DetectionReport>& DRs,
                                           DR_pair_rates_t& collection,
                                           std::set<DetectionReport>* pairedRemoved) const
{
  std::size_t cnt = 0;
  auto it = collection.begin();
  while (it != collection.end())
  {
    bool erased = false;
    for (auto& DR : DRs)
    {
      bool toRemove = false;
      if (std::get<0>(*it) == DR)
      {
        toRemove = true;
        if (pairedRemoved)
          pairedRemoved->insert(std::get<1>(*it));
      }
      else if (std::get<1>(*it) == DR)
      {
        toRemove = true;
        if (pairedRemoved)
          pairedRemoved->insert(std::get<0>(*it));
      }

      if (toRemove)
      {
        it = collection.erase(it);
        ++cnt;
        erased = true;
      }
    }
    if (!erased)
      ++it;
  }

  return cnt;
}

double
TrackManager::compare(const DetectionReport& l, const DetectionReport& r) const
{
  // calculates geometric distance between l and r (Euclidean)
  double lon_dist = l.getLongitude() - r.getLongitude();
  double lat_dist = l.getLatitude() - r.getLatitude();

  return 1/sqrt(pow(lon_dist,2) + pow(lat_dist,2)); // higher distance, lower rating -> closer DRs, better rating
}
