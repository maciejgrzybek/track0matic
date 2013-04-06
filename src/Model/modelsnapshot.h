#ifndef MODEL_SNAPSHOT_H
#define MODEL_SNAPSHOT_H

#include <memory>
#include <set>

#include <Model/track.h>

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

} // namespace Model

#endif // MODEL_SNAPSHOT_H
