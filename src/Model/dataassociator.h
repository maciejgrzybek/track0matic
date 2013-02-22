#ifndef DATAASSOCIATOR_H
#define DATAASSOCIATOR_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "detectionreport.h"
#include "featureextractor.h"
#include "resultcomparator.h"
#include "track.h"

class DataAssociator
{
public:

  /**
   * @brief Iterates over tracks and tries to associate DRs for each.
   *  iterates over tracks and invokes for each getListForTrack()
   *  DRs from returned chosen list are appended to associated map,
   *  "rejected" DRs are put into notAssociated list, by getListForTrack() method,
   *  if not found - are added to notAssociated.
   *  We cannot do it before finishing operations on all tracks, because rejected DR in context of one track,
   *  could be valid (properly associable) for other.
   */
  void compute();

  /**
   * @brief Gives map of Track->collection of corresponding DRs
   *  If necessary compute() will be invoked here.
   * @return Map of associations Track -> DRs
   */
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> > getDRsForTracks();

  /**
   * @brief If any DR is not associated to Track
   *  (i.e. new object's been seen in system), will be returned by this method.
   * @return Set of DRs which have no appropriate Track for them.
   */
  std::set<DetectionReport> getNotAssociated();

  /**
   * @brief Puts DRs to compute into DataAssociator.
   *  This method resets computed flag, so after invoking this method,
   *  any invokation of getDRsForTracks() or getNotAssociated(),
   *  will cause invoking compute()
   * @param collection of grouped (by neighborhood of sensors) DRs
   */
  void setInput(std::vector<std::set<DetectionReport> >);


  /**
   * @brief Sets ResultComparator, used to rate each DR for every track.
   *  According to rating, DR is matched to track or not.
   * @param Pointer to result comparator - we need polymorphism here. Takes ownership.
   */
  void setDRResultComparator(std::unique_ptr<ResultComparator> comparator);

  /**
   * @brief Sets ListResultComparator, used to rate each list of DRs, to check which is better.
   * @param Pointer to list result comparator - we need polymorphism here. Takes ownership.
   */
  void setListResultComparator(std::unique_ptr<ListResultComparator> comparator);

  /**
   * @brief Sets FeatureExtractor, used to associate DRs and rate associations.
   * @param FeatureExtractor
   */
  void setFeatureExtractor(std::unique_ptr<FeatureExtractor> extractor);

private:
  /**
   * @brief Returns best fit list of DRs for given track.
   *
   *  Iterates over input vector, getting sets of DRs,
   *  for each set (containing DRs from neighborhood) - rateListForTrack is invoked
   *  list (set) with best rate is chosen (each chosen DR is removed from input collection),
   *  "lost" DRs are left in collection, because will be saved into notAssociated vector,
   *  but before it happens, we need to finish iterations over tracks, because rejected DRs for one Track could matching for other.
   *
   *  IMPORTANT: We assume that one chosen DR cannot be associated for other Track.
   *    It's not best solution, because it may happen, that one list of DRs fits better for Track which will be checked later,
   *    but to allow rating of chosen lists for Track, we should keep track of every List (with it's grade) per Track.
   *    It's too expensive, so we do it greedy way on this level.
   *    E.g. when we have list of DRs: {1,2,3} which fits for Track with 0.7 imaginary grade,
   *    but there is other Track for which the same list - {1,2,3} would give 0.8 grade,
   *    it would be better to change association, that {1,2,3} is associated with this second Track, but this would imply
   *    finding new association for first Track (i.e. second on the grade list), which could be already used (associated to other Track) etc.
   *    It's too complex and would be bottleneck of whole Tracker, so we decide that if one list is chosen for Track,
   *    it cannot be assigned to any other Track anymore.
   *
   * @param Track for which we are looking associations for.
   * @return set of matching DRs
   * @see rateListForTrack
   */
  std::set<DetectionReport> getListForTrack(const Track&);

  /**
   * @brief Chooses these DRs from neighborhood, which are good enough, to match given track.
   *
   *  For each DR from list is invoked rateDRForTrack,
   *  DRs with rate above threshold, are returned as winning DRs, with rate. Rest are returned also, as "lost"
   * @param Set of DRs to choose from - is modified: each chosen DR is removed from this collection (it reduces complexity of computation, by so called sweeping)
   * @param Track to match DRs to
   * @return pair of two values: rate and collection of matching DRs
   */
  std::pair<double,std::set<DetectionReport> >
    rateListForTrack(std::set<DetectionReport>&,const Track&) const;

  /**
   * @brief Returns 0-1 grade for DR, in comparation for Track.
   *  when Track and DR class mismatch occurs (e.g. Track concerns car and DR human), rate = 0
   * @return grade
   */
  double rateDRForTrack(const DetectionReport&,const Track&) const;

  std::vector<std::set<DetectionReport> > DRGroups;
  std::map<Track,std::set<DetectionReport> > associatedDRs;
  std::unique_ptr<ResultComparator> resultComparator;
  std::unique_ptr<ListResultComparator> listResultComparator;
  std::unique_ptr<FeatureExtractor> featureExtractor;
};

#endif // DATAASSOCIATOR_H
