#ifndef TRACKMANAGER_H
#define TRACKMANAGER_H

#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <vector>

#include "detectionreport.h"
#include "featureextractor.h"
#include "track.h"

struct tuple_less
{
  bool operator()(const std::tuple<DetectionReport,DetectionReport,double>& l,
                  const std::tuple<DetectionReport,DetectionReport,double>& r) const
  {
    if (std::get<2>(l) < std::get<2>(r))
    {
      return true;
    }
    else if (std::get<2>(l) == std::get<2>(r))
    {
      std::less<DetectionReport> less;
      if (less(std::get<0>(l),std::get<0>(r)))
      {
        return true;
      }
      else if (std::get<0>(l) == std::get<0>(r))
      {
        return less(std::get<1>(l),std::get<1>(r));
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
};

class TrackManager
{
public:
  // TODO comment this
  TrackManager(double initializationThreshold);

  /**
   * @brief Iterates over collection of DRs groups (sets) to initialize Track for each group
   *
   *  Invokes initilizeTrack() method, for each group.
   * @param Collection of DRs (set), to be initilized.
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
  // mapping two DRs on rate (grade which implices their quality (based on distance etc.))
  typedef std::set<
    std::tuple<DetectionReport,DetectionReport,double>,
    tuple_less
    >DR_pair_rates_t;

  /**
   * @brief Create Track based on given Detection Reports.
   *  All given DRs has to be proper for Track, it is - TrackManager is not checking them for consistency,
   *  it will initilize them anyway.
   * @param Collection of Detection Reports to initialize Track from.
   * @param Estimation filter used to be assigned to Track
   * @return Track based on given DRs.
   */
  std::shared_ptr<Track> initializeTrack(const std::set<DetectionReport>&,
                                         std::unique_ptr<estimation::EstimationFilter<> >);

  std::pair<
  DR_pair_rates_t, // rates
  std::set<DetectionReport> // notAssigned
  >
  getRatedPairs(const std::set<DetectionReport>&) const;

  std::vector<std::set<DetectionReport> >
  chooseFromRated(std::pair<
                            DR_pair_rates_t, // rates
                            std::set<DetectionReport> // notAssigned
                           >) const;

  std::set<DetectionReport> getDRsFromTuples(const DR_pair_rates_t&) const;

  /**
   * @brief iterates over collection of tuples containing two DRs and their rating,
   *  buliding set of DRs, which has only these DRs which exist in pairs with given DR.
   * @return set of DRs connected with given one.
   */
  std::set<DetectionReport> getSetOfPairedDRs(const DetectionReport&,
                                              DR_pair_rates_t::const_iterator,
                                              DR_pair_rates_t::const_iterator) const;

  /**
   * @brief Removes pairs which contain given DR from collection passed by a reference.
   * @param DRs set containing DRs to be removed from tuples
   * @param Tuples collection to remove tuples containing DRs from given set from
   * @param optionally, pointer to collection where store removed paired DRs,
   *  e.g. when we have tuple (DR1,DR2,some_rating) and ask for removal tuples containing DR1,
   *  DR2 will be put here.
   * @return number of removed DRs
   */
  std::size_t
    removeDRsFromTupleCollection(const std::set<DetectionReport>&,
                                 DR_pair_rates_t&,
                                 std::set<DetectionReport>* = nullptr) const;

  /**
   * @brief Compares two DRs (typically by geometric, Euklidean distance)
   * @return rating of two DRs; higher - better
   */
  double compare(const DetectionReport&, const DetectionReport&) const;

  std::set<std::shared_ptr<Track> > tracks_;
  std::unique_ptr<FeatureExtractor> featureExtractor_;

  const double initializationDRThreshold_;

};

#endif // TRACKMANAGER_H
