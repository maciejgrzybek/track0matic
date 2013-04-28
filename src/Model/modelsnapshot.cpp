#include "modelsnapshot.h"

namespace Model
{

Snapshot::Snapshot(std::shared_ptr<std::set<std::unique_ptr<Track> > > data)
  : data_(data)
{}

std::shared_ptr<
                std::set<std::unique_ptr<Track> >
               > Snapshot::getData() const
{
  return data_;
}

/******************************************************************************/
/******************************************************************************/

WorldSnapshot::StreetNodeSnapshot::StreetNodeSnapshot(int id,
                                                      double lon,
                                                      double lat,
                                                      double mos)
  : nodeId(id),
    lon(lon),
    lat(lat),
    mos(mos)
{}

/******************************************************************************/

WorldSnapshot::StreetSnapshot
::StreetSnapshot(std::shared_ptr<WorldSnapshot::StreetNodeSnapshot> first,
                 std::shared_ptr<WorldSnapshot::StreetNodeSnapshot> second)
  : first(first),
    second(second)
{}

/******************************************************************************/

WorldSnapshot::WorldSnapshot(const MapPtr map)
{
  for (const StreetPtr street : map->edges)
  {
    std::shared_ptr<StreetNodeSnapshot> firstNodeSnapshot
        = nodeToSnapshot(street->first);
    vertexes.insert(firstNodeSnapshot);

    std::shared_ptr<StreetNodeSnapshot> secondNodeSnapshot
        = nodeToSnapshot(street->second);
    vertexes.insert(secondNodeSnapshot);

    edges.insert(
          std::make_shared<StreetSnapshot>(
            firstNodeSnapshot,secondNodeSnapshot)
          );
  }
}

std::shared_ptr<WorldSnapshot::StreetNodeSnapshot>
  WorldSnapshot::nodeToSnapshot(const StreetNodePtr node)
{
  return std::make_shared<StreetNodeSnapshot>(node->nodeId,
                                              node->lon.get(),
                                              node->lat.get(),
                                              node->mos.get());
}

} // namespace Model
