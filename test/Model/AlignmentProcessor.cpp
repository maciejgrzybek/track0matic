#define BOOST_TEST_DYN_LINK
#include <set>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Model/alignmentprocessor.h>
#include <Model/candidateselector.h>
#include <Model/detectionreport.h>
#include <Model/sensor.h>

BOOST_AUTO_TEST_SUITE( AlignmentProcessor_test )

namespace Helpers
{
  std::set<int> VectorToSet(const std::vector<DetectionReport>& vec)
  {
    std::set<int> result;
    for (auto& i : vec)
    {
      result.insert(i.getDrId());
    }
    return result;
  }
} // namespace Helpers

BOOST_AUTO_TEST_CASE( AlignmentTest1 )
{
  std::vector<DetectionReport> DRs
      = {
    DetectionReport(1,1,10,10,0,100,95),
    DetectionReport(1,2,20,20,0,120,95),
    DetectionReport(2,3,15,10,0,100,98),
    DetectionReport(3,4,30,30,0,150,110),
    DetectionReport(6,8,50,5,0,110,99),
    DetectionReport(5,7,40,35,5,120,109),
    DetectionReport(7,10,5,6,2,105,99)
        };

  AlignmentProcessor* ap = new AlignmentProcessor(boost::posix_time::seconds(1));
  ap->setDRsCollection(DRs);

  std::vector<DetectionReport> alignedVec = ap->getNextAlignedGroup();
  std::set<int> aligned = Helpers::VectorToSet(alignedVec);
  BOOST_CHECK_EQUAL(aligned.size(),2);
  // first group has to contain DRs with id = 1 and 2
  BOOST_CHECK(aligned.find(1) != aligned.end());
  BOOST_CHECK(aligned.find(2) != aligned.end());

  alignedVec = ap->getNextAlignedGroup();
  aligned = Helpers::VectorToSet(alignedVec);
  BOOST_CHECK_EQUAL(aligned.size(),3);
  BOOST_CHECK(aligned.find(3) != aligned.end());
  BOOST_CHECK(aligned.find(8) != aligned.end());
  BOOST_CHECK(aligned.find(10) != aligned.end());

  alignedVec = ap->getNextAlignedGroup();
  aligned = Helpers::VectorToSet(alignedVec);
  BOOST_CHECK_EQUAL(aligned.size(),2);
  BOOST_CHECK(aligned.find(4) != aligned.end());
  BOOST_CHECK(aligned.find(7) != aligned.end());

  delete ap;
}

BOOST_AUTO_TEST_CASE( AlignmentTest2 )
{
  std::vector<DetectionReport> DRs;

  AlignmentProcessor* ap = new AlignmentProcessor(boost::posix_time::seconds(1));
  ap->setDRsCollection(DRs);
  std::vector<DetectionReport> aligned = ap->getNextAlignedGroup();
  BOOST_CHECK_EQUAL(aligned.size(),0);

  aligned = ap->getNextAlignedGroup(); // should not fail/throw etc.
  BOOST_CHECK_EQUAL(aligned.size(),0);

  delete ap;
}

BOOST_AUTO_TEST_CASE( AlignmentTest3 )
{
  std::vector<DetectionReport> DRs
      = {
    DetectionReport(1,1,10,10,0,100,95),
    DetectionReport(1,2,20,20,0,120,95),
    DetectionReport(2,3,15,10,0,100,98),
    DetectionReport(3,4,30,30,0,150,110),
    DetectionReport(6,8,50,5,0,110,99),
    DetectionReport(5,7,40,35,5,120,109),
    DetectionReport(7,10,5,6,2,105,99)
        };

  AlignmentProcessor* ap = new AlignmentProcessor(boost::posix_time::seconds(3));
  ap->setDRsCollection(DRs);

  std::vector<DetectionReport> alignedVec = ap->getNextAlignedGroup();
  std::set<int> aligned = Helpers::VectorToSet(alignedVec);
  BOOST_CHECK_EQUAL(aligned.size(),3);
  BOOST_CHECK(aligned.find(1) != aligned.end());
  BOOST_CHECK(aligned.find(2) != aligned.end());
  BOOST_CHECK(aligned.find(3) != aligned.end());

  alignedVec = ap->getNextAlignedGroup();
  aligned = Helpers::VectorToSet(alignedVec);
  BOOST_CHECK_EQUAL(aligned.size(),2);
  BOOST_CHECK(aligned.find(8) != aligned.end());
  BOOST_CHECK(aligned.find(10) != aligned.end());

  alignedVec = ap->getNextAlignedGroup();
  aligned = Helpers::VectorToSet(alignedVec);
  BOOST_CHECK_EQUAL(aligned.size(),2);
  BOOST_CHECK(aligned.find(4) != aligned.end());
  BOOST_CHECK(aligned.find(7) != aligned.end());

  delete ap;
}

BOOST_AUTO_TEST_CASE( AlignmentTest4 )
{
  std::vector<DetectionReport> DRs
      = {
    DetectionReport(1,1,10,10,0,100,95),
    DetectionReport(1,2,20,20,0,120,95),
    DetectionReport(2,3,15,10,0,100,98),
    DetectionReport(3,4,30,30,0,150,110),
    DetectionReport(6,8,50,5,0,110,99),
    DetectionReport(5,7,40,35,5,120,109),
    DetectionReport(7,10,5,6,2,105,99)
        };

  AlignmentProcessor* ap = new AlignmentProcessor(boost::posix_time::seconds(20));
  ap->setDRsCollection(DRs);

  std::vector<DetectionReport> alignedVec = ap->getNextAlignedGroup();
  std::set<int> aligned = Helpers::VectorToSet(alignedVec);
  BOOST_CHECK_EQUAL(aligned.size(),7);
  BOOST_CHECK(aligned.find(1) != aligned.end());
  BOOST_CHECK(aligned.find(2) != aligned.end());
  BOOST_CHECK(aligned.find(3) != aligned.end());
  BOOST_CHECK(aligned.find(4) != aligned.end());
  BOOST_CHECK(aligned.find(7) != aligned.end());
  BOOST_CHECK(aligned.find(8) != aligned.end());
  BOOST_CHECK(aligned.find(10) != aligned.end());

  alignedVec = ap->getNextAlignedGroup();
  BOOST_CHECK_EQUAL(alignedVec.size(),0);

  delete ap;
}

BOOST_AUTO_TEST_SUITE_END()

