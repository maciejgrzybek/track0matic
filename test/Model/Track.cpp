#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Model/estimationfilter.hpp>
#include <Model/track.h>

BOOST_AUTO_TEST_SUITE( Track_test )

namespace Track_test
{
  struct Fixture
  {
    Fixture()
    {
      { // FIXME: really UGLY solution! Only for testing purpose,
        //  to allow fast tests
        #include "common/FiltersSetups.h"
        filter = std::move(kalmanFilter);
      }
    }

    std::unique_ptr<estimation::EstimationFilter<> > filter;
  };

} // namespace Track_test

BOOST_FIXTURE_TEST_CASE( Track_expiration_simple_test, Track_test::Fixture )
{
  time_types::ptime_t p1(boost::chrono::seconds(1)); // 1s after epoch

  Track* t = new Track(filter->clone(),0,1,2,p1);

  // track initialized already

  // test time points and durations preparation:

  time_types::duration_t duration0(boost::chrono::seconds(0)); // Track expires after 0s without refresh
  time_types::duration_t duration1(boost::chrono::seconds(1)); // Track expires after 1s without refresh
  time_types::duration_t duration2(boost::chrono::seconds(2)); // Track expires after 2s without refresh

  time_types::ptime_t current1(boost::chrono::seconds(1)); // 1s after epoch
  time_types::ptime_t current2(boost::chrono::seconds(2)); // 2s after epoch
  time_types::ptime_t current4(boost::chrono::seconds(4)); // 4s after epoch

  // current time is epoch+1s
  BOOST_CHECK(t->isTrackValid(current1,duration0)); // Track refresh time is current time - track is always valid
  BOOST_CHECK(t->isTrackValid(current1,duration2)); // Track refresh time is current time - track is valid with TTL = 2s

  // current time is epoch+2s
  BOOST_CHECK(!t->isTrackValid(current2,duration0)); // Current time is 1s after Track's refresh time, TTL allows 0s without refresh - track is invalid
  BOOST_CHECK(t->isTrackValid(current2,duration1)); // Current time is 1s after Track's refresh time, TTL allows 1s without refresh
  BOOST_CHECK(t->isTrackValid(current2,duration2)); // Current time is 1s after Track's refresh time, TTL allows 2s without refresh

  // current time is epoch+4s
  BOOST_CHECK(!t->isTrackValid(current4,duration0)); // Current time is 3s after Track's refresh time, TTL allows 0s without refresh - track is invalid
  BOOST_CHECK(!t->isTrackValid(current4,duration1)); // Current time is 3s after Track's refresh time, TTL allows 1s without refresh - track is invalid
  BOOST_CHECK(!t->isTrackValid(current4,duration2)); // Current time is 3s after Track's refresh time, TTL allows 2s without refresh - track is invalid

  delete t;
}

BOOST_AUTO_TEST_SUITE_END()

