#ifndef MODEL_SNAPSHOT_H
#define MODEL_SNAPSHOT_H

#include <memory>
#include <set>

#include <Model/track.h>

#include <3rdparty/DBDataStructures.h> // for Model::MapPtr

namespace Model
{

class Snapshot
{
public:
  Snapshot() = default; // to allow putting in collections
  Snapshot(std::shared_ptr<
                           std::set<std::unique_ptr<Track> >
                          >);
  std::shared_ptr<
                  std::set<std::unique_ptr<Track> >
                 > getData() const;

private:
  std::shared_ptr<
                  std::set<std::unique_ptr<Track> >
                 > data_;
};

class WorldSnapshot
{
public:
  struct StreetNodeSnapshot
  {
    StreetNodeSnapshot(int id, double lon, double lat, double mos);

    int nodeId;
    double lon; // losing constraints
    double lat;
    double mos;
  };

  struct StreetSnapshot
  {
    StreetSnapshot(std::shared_ptr<StreetNodeSnapshot> first,
                   std::shared_ptr<StreetNodeSnapshot> second);

    std::shared_ptr<StreetNodeSnapshot> first;
    std::shared_ptr<StreetNodeSnapshot> second;
  };

  WorldSnapshot() = default; // to allow putting in collections
  /**
   * @brief WorldSnapshot c-tor, transforms MapPtr to WorldSnapshot.
   * @param MapPtr - Model's struct describing map.
   */
  explicit WorldSnapshot(const MapPtr); // copies only streets!

  std::set<std::shared_ptr<StreetNodeSnapshot> > vertexes;
  std::set<std::shared_ptr<StreetSnapshot> > edges;

private:
  std::shared_ptr<StreetNodeSnapshot> nodeToSnapshot(const StreetNodePtr);
};

} // namespace Model

#endif // MODEL_SNAPSHOT_H
