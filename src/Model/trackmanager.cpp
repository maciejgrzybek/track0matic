#include "trackmanager.h"

std::map<Track,std::set<DetectionReport> >
  TrackManager::initializeTracks(const std::vector<std::set<DetectionReport> >& DRsGroups)
{
  // for each set from DRsGroups, invoke initializeTrack() and add returned Track to tracks_ set.
  // All initialized Tracks are collected and returned as map Track->DRs.
  std::map<Track,std::set<DetectionReport> > result;
  for (auto& DRGroup : DRsGroups)
  {
    std::shared_ptr<Track> track = initializeTrack(DRGroup);
    tracks_.insert(track);

    result[track] = DRGroup;
  }

  return result;
}

void TrackManager::setFeatureExtractor(std::unique_ptr<FeatureExtractor> extractor)
{
  featureExtractor_ = std::move(extractor);
}

std::shared_ptr<Track> TrackManager::initializeTrack(const std::set<DetectionReport>& DRs,
                                                     std::unique_ptr<estimation::EstimationFilter> filter)
{
  // TODO implement merging features from DRs for Track (based on FeatureExtractor's fuse() method).

  double lons = 0;
  double lats = 0;
  double moses = 0;
  std::size_t cnt = 0;
  boost::posix_time::ptime maxTime;
  for (auto& DR : DRs)
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

  std::shared_ptr<Track> track(new Track(filter->clone(),lon,lat,mos,DRTime));

  return track;
}
