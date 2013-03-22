#define BOOST_TEST_DYN_LINK
#include <set>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Model/dataassociator.h>
#include <Model/detectionreport.h>
#include <Model/estimationfilter.hpp>

BOOST_AUTO_TEST_SUITE( DataAssociator_test_suite )

namespace Helpers
{
  std::set<int> SetToSet(const std::set<DetectionReport>& vec)
  {
    std::set<int> result;
    for (auto& i : vec)
    {
      result.insert(i.getDrId());
    }
    return result;
  }

  struct Fixture
  {
    Fixture()
      : tm_(new TrackManager(1))
    {
      { // FIXME: really UGLY solution! Only for testing purpose,
        //  to allow fast tests
        #include "common/FiltersSetups.h"
        filter_ = std::move(kalmanFilter);
      }

      std::set<DetectionReport> group = {
        DetectionReport(1,1,0,0,0,100,95),
        DetectionReport(2,2,1,0,0,100,95),
        DetectionReport(2,3,1,1,0,91,90),
        DetectionReport(3,4,0,1,0,105,99)
      };

      groups_.push_back(group);
      // these DRs will produce two Tracks: {1,4} and {2,3}
      tm_->initializeTracks(groups_,std::move(filter_));

      std::unique_ptr<DataAssociator> da(new DataAssociator(
            std::move(tm_),
            std::unique_ptr<ResultComparator>(new OrComparator(grade_map_)),
            std::unique_ptr<ListResultComparator>(new OrListComparator())
          ));
      da_ = std::move(da);
    }

    std::vector<std::set<DetectionReport> > groups_;
    std::unique_ptr<TrackManager> tm_;
    std::unique_ptr<estimation::EstimationFilter<> > filter_;
    ResultComparator::feature_grade_map_t grade_map_;
    std::unique_ptr<DataAssociator> da_;
  };

  bool compare(const std::set<DetectionReport>& l,
               const std::set<DetectionReport>& r)
  {
    if (l.size() != r.size())
      return false;

    auto lIt = l.begin();
    auto rIt = r.begin();
    for (;lIt != l.end(); ++lIt, ++rIt) // does not have to check rIt != r.end(),
                                       // because of above check
    {
      if (!(*lIt == *rIt))
        return false;
    }

    return true;
  }

  // checks whether each element (set) from results exists in correct (ommits order).
  bool checkConsistency(const std::vector<std::set<DetectionReport> >& results,
                        const std::vector<std::set<DetectionReport> >& correct)
  {
    if (results.size() != correct.size())
      return false;

    for (const std::set<DetectionReport>& r : results)
    {
      // if there is no correct item like result
      if (std::find(correct.begin(),correct.end(),r) == correct.end())
        return false; // test failed
    }
    return true;
  }

} // namespace Helpers

BOOST_FIXTURE_TEST_CASE( DataAssociator_simple_test, Helpers::Fixture )
{
  std::vector<std::set<DetectionReport> > DRsGroups;
  std::set<DetectionReport> group = {
    DetectionReport(1,8,0,0.1,0,100,95), // should be assigned to Track with DRs {1,4}
    DetectionReport(2,9,1.0,0,0,100,95) // should be assigned to Track with DRs {2,3}
  };
  DRsGroups.push_back(group);

  std::vector<std::set<DetectionReport> > correct;
  {
    std::set<DetectionReport> correctFirst = {
      DetectionReport(1,8,0,0.1,0,100,95)
    };
    correct.push_back(correctFirst);
  }
  {
    std::set<DetectionReport> correctSecond = {
      DetectionReport(2,9,1.0,0,0,100,95)
    };
    correct.push_back(correctSecond);
  }

  da_->setInput(DRsGroups);
  da_->setDRRateThreshold(0.1);
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> >
      assigned = da_->getDRsForTracks();

  BOOST_REQUIRE_EQUAL(assigned.size(),2);
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> >::const_iterator
      it = assigned.begin(); // first Track->DRs association
  BOOST_REQUIRE_EQUAL(it->second.size(),1); // if there are no DRs in set,
                                            // we cannot check them (DRs)

  std::vector<std::set<DetectionReport> > results;
  results.push_back(it->second);
  ++it; // second association

  BOOST_REQUIRE_EQUAL(it->second.size(),1); // if there are no DRs in set,
                                            // we cannot check them (DRs)
  results.push_back(it->second);

  BOOST_CHECK(Helpers::checkConsistency(results,correct));
}

BOOST_FIXTURE_TEST_CASE( DataAssociator_simple_test2, Helpers::Fixture )
{
  std::vector<std::set<DetectionReport> > DRsGroups;
  std::set<DetectionReport> group = {
    DetectionReport(1,8,0,0.1,0,100,95), // should be assigned to Track with DRs {1,4}
    DetectionReport(2,9,1.0,0,0,100,95), // should be assigned to Track with DRs {2,3}
    DetectionReport(1,10,0,0.6,0,100,105), // should be assigned to Track with DRs {1,4}
    DetectionReport(2,11,1.0,0.1,0,99,101), // should be assigned to Track with DRs {2,3}
  };
  DRsGroups.push_back(group);

  std::vector<std::set<DetectionReport> > correct;
  {
    std::set<DetectionReport> correctFirst = {
      DetectionReport(1,8,0,0.1,0,100,95),
      DetectionReport(1,10,0,0.6,0,100,105)
    };
    correct.push_back(correctFirst);
  }

  {
    std::set<DetectionReport> correctSecond = {
      DetectionReport(2,9,1.0,0,0,100,95),
      DetectionReport(2,11,1.0,0.1,0,99,101)
    };
    correct.push_back(correctSecond);
  }

  da_->setInput(DRsGroups);
  da_->setDRRateThreshold(0.1);
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> >
      assigned = da_->getDRsForTracks();

  BOOST_REQUIRE_EQUAL(assigned.size(),2);
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> >::const_iterator
      it = assigned.begin(); // first Track->DRs association

  BOOST_REQUIRE_EQUAL(it->second.size(),2);

  std::vector<std::set<DetectionReport> > results;
  results.push_back(it->second);
  ++it; // second association

  BOOST_REQUIRE_EQUAL(it->second.size(),2);

  results.push_back(it->second);

  BOOST_CHECK(Helpers::checkConsistency(results,correct));
}

BOOST_AUTO_TEST_CASE( DataAssociator_track_refresh_on_association_test )
{
/*****************************INITIALIZATION***********************************/

  TrackManager* tm_ = new TrackManager(1);
  std::unique_ptr<estimation::EstimationFilter<> > filter_;

  { // FIXME: really UGLY solution! Only for testing purpose,
    //  to allow fast tests
    #include "common/FiltersSetups.h"
    filter_ = std::move(kalmanFilter);
  }

  std::set<DetectionReport> group = {
    DetectionReport(1,1,0,0,0,100,95),
    DetectionReport(2,2,1,0,0,100,95),
    DetectionReport(2,3,1,1,0,91,90),
    DetectionReport(3,4,0,1,0,105,99)
  };

  std::vector<std::set<DetectionReport> > groups_;
  groups_.push_back(group);
  // these DRs will produce two Tracks: {1,4} and {2,3}
  tm_->initializeTracks(groups_,std::move(filter_));

  ResultComparator::feature_grade_map_t grade_map_;

  DataAssociator* da_ = new DataAssociator(
        std::unique_ptr<TrackManager>(tm_), // ugly, but needed for testing
       // purpose (to allow reading data from already passed to DA TrackManager)
        std::unique_ptr<ResultComparator>(new OrComparator(grade_map_)),
        std::unique_ptr<ListResultComparator>(new OrListComparator())
      );

/**********************************Association*********************************/
  std::vector<std::set<DetectionReport> > DRsGroups;
  {
    std::set<DetectionReport> group = {
      DetectionReport(1,8,0,0.1,0,100,95), // should be assigned to Track with DRs {1,4}
      DetectionReport(2,9,1.0,0,0,100,95), // should be assigned to Track with DRs {2,3}
      DetectionReport(1,10,0,0.6,0,100,105), // should be assigned to Track with DRs {1,4}
      DetectionReport(2,11,1.0,0.1,0,99,101), // should be assigned to Track with DRs {2,3}
    };
    DRsGroups.push_back(group);
  }

  da_->setInput(DRsGroups);
  da_->setDRRateThreshold(0.1);
  da_->compute();

/******************************************************************************/

  // remove tracks not refreshed since epoch+101s (105-1). Should remove nothing
  std::size_t removedCnt
    = tm_->removeExpiredTracks(time_types::ptime_t(boost::chrono::seconds(105)),
                           time_types::duration_t(4));

  BOOST_CHECK_EQUAL(removedCnt,0);
  BOOST_CHECK_EQUAL(tm_->getTracksRef().size(),2);

  // remove tracks not refreshed since epoch+105s (106-1). Should remove one Track
  removedCnt
    = tm_->removeExpiredTracks(time_types::ptime_t(boost::chrono::seconds(106)),
                           time_types::duration_t(1));

  // only one Track left
  BOOST_CHECK_EQUAL(removedCnt,1);
  const std::set<std::shared_ptr<Track> >& tracks = tm_->getTracksRef();
  BOOST_REQUIRE_EQUAL(tracks.size(),1);

  // getting first element from set is safe, because of above REQUIREd statement
  std::shared_ptr<Track> track = *tracks.begin();

  BOOST_CHECK_EQUAL(track->getRefreshTime(),
                    time_types::ptime_t(boost::chrono::seconds(105)));
}

BOOST_AUTO_TEST_SUITE_END()

