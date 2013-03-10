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
      : tm_(new TrackManager(1)),
        filter_(new estimation::KalmanFilter<>())
    {
      std::set<DetectionReport> group = {
        DetectionReport(1,1,0,0,0,100,95),
        DetectionReport(2,2,1,0,0,100,95),
        DetectionReport(2,3,1,1,0,91,90),
        DetectionReport(3,4,0,1,0,105,99)
      };

      groups_.push_back(group);
      // these DRs will produce two Tracks: {1,4} and {2,3}
      tm_->initializeTracks(groups_,
                            std::unique_ptr<estimation::EstimationFilter<> >
                            (filter_));
    }

    std::vector<std::set<DetectionReport> > groups_;
    std::unique_ptr<TrackManager> tm_;
    estimation::EstimationFilter<>* filter_;
    ResultComparator::feature_grade_map_t grade_map_;
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
    DetectionReport(2,9,1.01,0,0,100,95) // should be assigned to Track with DRs {2,3}
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
      DetectionReport(2,9,1.01,0,0,100,95)
    };
    correct.push_back(correctSecond);
  }

  DataAssociator* da
      = new DataAssociator(
          std::move(tm_),
          std::unique_ptr<ResultComparator>(new OrComparator(grade_map_)),
          std::unique_ptr<ListResultComparator>(new OrListComparator())
        );

  da->setInput(DRsGroups);
  da->setDRRateThreshold(0.1);
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> >
      assigned = da->getDRsForTracks();

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

  delete da;
}

BOOST_FIXTURE_TEST_CASE( DataAssociator_simple_test2, Helpers::Fixture )
{
  std::vector<std::set<DetectionReport> > DRsGroups;
  std::set<DetectionReport> group = {
    DetectionReport(1,8,0,0.1,0,100,95), // should be assigned to Track with DRs {1,4}
    DetectionReport(2,9,1.01,0,0,100,95), // should be assigned to Track with DRs {2,3}
    DetectionReport(1,10,0,0.6,0,100,105), // should be assigned to Track with DRs {1,4}
    DetectionReport(2,11,0.96,0,0,99,101), // should be assigned to Track with DRs {2,3}
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
      DetectionReport(2,9,1.01,0,0,100,95),
      DetectionReport(2,11,0.96,0,0,99,101)
    };
    correct.push_back(correctSecond);
  }

  DataAssociator* da
      = new DataAssociator(
          std::move(tm_),
          std::unique_ptr<ResultComparator>(new OrComparator(grade_map_)),
          std::unique_ptr<ListResultComparator>(new OrListComparator())
        );

  da->setInput(DRsGroups);
  da->setDRRateThreshold(0.1);
  std::map<std::shared_ptr<Track>,std::set<DetectionReport> >
      assigned = da->getDRsForTracks();

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

  delete da;
}

BOOST_AUTO_TEST_SUITE_END()

