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
      : p1(boost::chrono::seconds(1)), // 1s after epoch
        p2(boost::chrono::seconds(2)), // 2s after epoch
        p4(boost::chrono::seconds(4)), // 4s after epoch
        p8(boost::chrono::seconds(8)), // 8s after epoch
        duration0(boost::chrono::seconds(0)), // 0s TTL - // Track expires after 0s without refresh
        duration1(boost::chrono::seconds(1)), // 1s TTL
        duration2(boost::chrono::seconds(2)) // 2s TTL
    {
      { // FIXME: really UGLY solution! Only for testing purpose,
        //  to allow fast tests
        #include "common/FiltersSetups.h"
        filter = std::move(kalmanFilter);
      }
    }

    std::unique_ptr<estimation::EstimationFilter<> > filter;

    time_types::ptime_t p1;
    time_types::ptime_t p2;
    time_types::ptime_t p4;
    time_types::ptime_t p8;

    time_types::duration_t duration0;
    time_types::duration_t duration1;
    time_types::duration_t duration2;
  };

} // namespace Track_test

BOOST_FIXTURE_TEST_CASE( Track_expiration_simple_test, Track_test::Fixture )
{
  Track* t = new Track(filter->clone(),0,1,2,0,0,0,p1);

  // track initialized already

  // test time points and durations preparation:

  // current time is epoch+1s
  BOOST_CHECK(t->isTrackValid(p1,duration0)); // Track refresh time is current time - track is always valid
  BOOST_CHECK(t->isTrackValid(p1,duration2)); // Track refresh time is current time - track is valid with TTL = 2s

  // current time is epoch+2s
  BOOST_CHECK(!t->isTrackValid(p2,duration0)); // Current time is 1s after Track's refresh time, TTL allows 0s without refresh - track is invalid
  BOOST_CHECK(t->isTrackValid(p2,duration1)); // Current time is 1s after Track's refresh time, TTL allows 1s without refresh
  BOOST_CHECK(t->isTrackValid(p2,duration2)); // Current time is 1s after Track's refresh time, TTL allows 2s without refresh

  // current time is epoch+4s
  BOOST_CHECK(!t->isTrackValid(p4,duration0)); // Current time is 3s after Track's refresh time, TTL allows 0s without refresh - track is invalid
  BOOST_CHECK(!t->isTrackValid(p4,duration1)); // Current time is 3s after Track's refresh time, TTL allows 1s without refresh - track is invalid
  BOOST_CHECK(!t->isTrackValid(p4,duration2)); // Current time is 3s after Track's refresh time, TTL allows 2s without refresh - track is invalid

  delete t;
}

BOOST_FIXTURE_TEST_CASE( Track_refreshing_simple_test, Track_test::Fixture )
{
  // refresh time of newly created Track is epoch+1s
  Track* t = new Track(filter->clone(),0,1,2,0,0,0,p1);

  // track initialized already

  // test time points and durations preparation:

  // current time is epoch+2s
  BOOST_CHECK(t->isTrackValid(p2,duration2)); // TTL 2s, track is valid
  // times passes...
  t->refresh(p4); // track refresh
  // current time is epoch+4s
  BOOST_CHECK(t->isTrackValid(p4,duration2)); // TTL 2s, track is still valid, because of refresh
  // time passes...
  BOOST_CHECK(!t->isTrackValid(p8,duration2)); // track is invalid (expired), because was not refreshed till p4
                          // was valid only till p6 (p4 + 2s TTL)

  delete t;
}

BOOST_AUTO_TEST_SUITE_END()

