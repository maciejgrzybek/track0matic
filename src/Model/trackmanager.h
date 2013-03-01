#ifndef TRACKMANAGER_H
#define TRACKMANAGER_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "detectionreport.h"
#include "featureextractor.h"
#include "track.h"

class TrackManager
{
public:
  /**
   * @brief Iterates over collection of DRs groups (sets) to initialize Track for each group
   *
   *  Invokes initilizeTrack() method, for each group.
   * @param Collection of DRs sets, to be initilized. Each set results in initialization as a separate Track.
   * @param Estimation filter to be used for initialized Track.
   * @return Track->DRs map, where Track is based on given DRs.
   */
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> >
    initializeTracks(const std::vector<std::set<DetectionReport> >&,
                     std::unique_ptr<estimation::EstimationFilter<> >);

  /**
   * @brief Sets FeatureExtractor, used to merge (fuse) DRs' features
   * @param FeatureExtractor - TrackManager takes ownership of this!
   */
  void setFeatureExtractor(std::unique_ptr<FeatureExtractor> extractor);

private:
  /**
   * @brief Create Track based on given Detection Reports.
   *  All given DRs has to be proper for Track, it is - TrackManager is not checking them for consistency,
   *  it will initilize them anyway.
   * @param Collection of Detection Reports to initialize Track from.
   * @return Track based on given DRs.
   */
  std::shared_ptr<Track> initializeTrack(const std::set<DetectionReport>&);

  std::set<std::shared_ptr<Track> > tracks_;
  std::unique_ptr<FeatureExtractor> featureExtractor_;

};

#endif // TRACKMANAGER_H
