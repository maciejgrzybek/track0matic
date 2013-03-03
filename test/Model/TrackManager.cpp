#define BOOST_TEST_DYN_LINK

#include <memory>
#include <set>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Model/estimationfilter.hpp>
#include <Model/featureextractor.h>
#include <Model/trackmanager.h>

BOOST_AUTO_TEST_SUITE( TrackManager_test )

namespace TrackManager_test
{
  struct Fixture
  {
    Fixture()
      : featureExtractor(new FeatureExtractor()),
        filter(new estimation::KalmanFilter<>())
    {
    }

    FeatureExtractor* featureExtractor;
    estimation::EstimationFilter<>* filter;
  };

  std::vector<std::set<DetectionReport> >
      mapToVec(std::map<std::shared_ptr<Track>,std::set<DetectionReport> >& m)
  {
    std::vector<std::set<DetectionReport> > v;
    for (auto& i : m)
    {
      v.push_back(i.second);
    }
    return v;
  }

  void checkConsistency(std::map<std::shared_ptr<Track>,
                        std::set<DetectionReport> > result,
                        std::vector<std::set<DetectionReport> > correctResult)
  {
    std::vector<std::set<DetectionReport> > v = mapToVec(result);

    // check whether in vector there are appropriate sets (as in correctResult)
    BOOST_REQUIRE_EQUAL(v.size(),correctResult.size()); // can't use EQUAL_COLLECTIONS, because we does not care of order in them
    for (auto& s : v)
    {
      bool match = false;
      for (auto& r : correctResult)
      {
        if ((r == s) && (!match))
        {
          match = true;
        }
        else if (r == s) // if matched again, failure (doubled sets)
        {
          match = false;
        }
      }
      BOOST_CHECK(match); // each set from result, has to meet requirements (look above, inner loop)
    }
  }
} // namespace TrackManager_test

BOOST_FIXTURE_TEST_CASE( TrackInitialization_simple_groups, TrackManager_test::Fixture )
{
  TrackManager* tm = new TrackManager(1);
  tm->setFeatureExtractor(std::unique_ptr<FeatureExtractor>(featureExtractor));

  std::vector<std::set<DetectionReport> > groups;
  {
    std::set<DetectionReport> group = {
      DetectionReport(1,1,0,0,0,100,95),
      DetectionReport(2,2,1,0,0,100,95),
      DetectionReport(2,3,1,1,0,91,90),
      DetectionReport(3,4,0,1,0,105,99)
    };
    groups.push_back(group);
  }
  {
    std::set<DetectionReport> group = {
      DetectionReport(4,5,3,1,0,100,96),
      DetectionReport(4,6,4,0,0,101,92)
    };
    groups.push_back(group);
  }
  {
    std::set<DetectionReport> group = {
      DetectionReport(5,7,5,3,0,104,103)
    };
    groups.push_back(group);
  }

  // correct result (as set, instead of map)
  std::vector<std::set<DetectionReport> > correctResult;
  {
    std::set<DetectionReport> trackDRs
        = { DetectionReport(2,2,1,0,0,100,95),
            DetectionReport(2,3,1,1,0,91,90) };
    correctResult.push_back(trackDRs);
  }
  {
    std::set<DetectionReport> trackDRs
        = { DetectionReport(1,1,0,0,0,100,95),
            DetectionReport(3,4,0,1,0,105,99) };
    correctResult.push_back(trackDRs);
  }
  {
    std::set<DetectionReport> trackDRs
        = { DetectionReport(4,5,3,1,0,100,96) };
    correctResult.push_back(trackDRs);
  }
  {
    std::set<DetectionReport> trackDRs
        = { DetectionReport(4,6,4,0,0,101,92) };
    correctResult.push_back(trackDRs);
  }
  {
    std::set<DetectionReport> trackDRs
        = { DetectionReport(5,7,5,3,0,104,103) };
    correctResult.push_back(trackDRs);
  }

  std::map<std::shared_ptr<Track>,
      std::set<DetectionReport> > result
      = tm->initializeTracks(groups,
                             std::unique_ptr<estimation::EstimationFilter<> >(filter));

  TrackManager_test::checkConsistency(result,correctResult);

  delete tm;
}

BOOST_FIXTURE_TEST_CASE( TrackInitialization_simple_merging, TrackManager_test::Fixture )
{
  TrackManager* tm = new TrackManager(0.5);
  tm->setFeatureExtractor(std::unique_ptr<FeatureExtractor>(featureExtractor));

  std::vector<std::set<DetectionReport> > groups;
  {
    std::set<DetectionReport> group = {
      DetectionReport(1,1,0,0,0,100,95),
      DetectionReport(2,2,2,0,0,100,95),
      DetectionReport(2,3,1,1,0,91,90)
    };
    groups.push_back(group);
  }

  // correct result (as set, instead of map)
  std::vector<std::set<DetectionReport> > correctResult;
  {
    std::set<DetectionReport> trackDRs
        = { DetectionReport(1,1,0,0,0,100,95),
            DetectionReport(2,2,2,0,0,100,95),
            DetectionReport(2,3,1,1,0,91,90) };
    correctResult.push_back(trackDRs);
  }

  std::map<std::shared_ptr<Track>,
      std::set<DetectionReport> > result
      = tm->initializeTracks(groups,
                             std::unique_ptr<estimation::EstimationFilter<> >(filter));

  TrackManager_test::checkConsistency(result,correctResult);

  delete tm;
}

BOOST_FIXTURE_TEST_CASE( TrackInitialization_exclusive_merging, TrackManager_test::Fixture )
{
  TrackManager* tm = new TrackManager(0.6);
  tm->setFeatureExtractor(std::unique_ptr<FeatureExtractor>(featureExtractor));

  std::vector<std::set<DetectionReport> > groups;
  {
    std::set<DetectionReport> group = {
      DetectionReport(1,1,0,0,0,100,95),
      DetectionReport(2,2,2,0,0,100,95),
      DetectionReport(2,3,1,1,0,91,90)
    };
    groups.push_back(group);
  }

  // correct result (as set, instead of map)
  std::vector<std::set<DetectionReport> > correctResult;
  {
    std::set<DetectionReport> trackDRs
        = { DetectionReport(1,1,0,0,0,100,95),
            DetectionReport(2,3,1,1,0,91,90) };
    correctResult.push_back(trackDRs);
  }
  {
    std::set<DetectionReport> trackDRs
        = { DetectionReport(2,2,2,0,0,100,95) };
    correctResult.push_back(trackDRs);
  }

  std::map<std::shared_ptr<Track>,
      std::set<DetectionReport> > result
      = tm->initializeTracks(groups,
                             std::unique_ptr<estimation::EstimationFilter<> >(filter));

  TrackManager_test::checkConsistency(result,correctResult);

  delete tm;
}

BOOST_AUTO_TEST_SUITE_END()

