#include <Model/datamanager.h>

// DEPRECATED, gnuplot.cpp will be removed soon
int main(void)
{
  DataManager dm;
  std::shared_ptr<
      std::set<std::shared_ptr<Track> >
    > result = dm.computeTracks();

  std::cout << "Tracks:" << std::endl;
  for (auto t : *result)
  {
    std::cout << "Track: " << t->getLongitude() <<","<<t->getLatitude() << std::endl;
  }

  return 0;
}
