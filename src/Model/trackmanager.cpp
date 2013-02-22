#include "trackmanager.h"

std::map<Track,std::set<DetectionReport> >
  TrackManager::initializeTracks(const std::vector<std::set<DetectionReport> >& DRsGroups)
{
  // TODO implement this
  // for each set from DRsGroups, invoke initializeTrack() and add returned Track to tracks_ set.
  // All initialized Tracks are collected and returned as map Track->DRs.
}

void TrackManager::setFeatureExtractor(std::unique_ptr<FeatureExtractor> extractor)
{
  featureExtractor_ = std::move(extractor);
}

std::shared_ptr<Track> TrackManager::initializeTrack(const std::set<DetectionReport>& DRs)
{
  // TODO implement this
  // merge DRs: attributes and features (basing on FeatureExtractor's fuse() method)
}
