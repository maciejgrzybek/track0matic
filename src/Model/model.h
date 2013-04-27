#ifndef MODEL_H
#define MODEL_H

#include <Model/modelsnapshot.h>

#include <3rdparty/DBDataStructures.h>

namespace Model
{

class Model
{
public:
  virtual ~Model()
  {}

  /**
   * @brief Computes next state of Model.
   * @param Current time - time passing indicator in Model.
   *  When default value is used, refresh time of the latest Track is used.
   * @return Snapshot of Model after computation.
   */
  virtual Snapshot computeState(time_types::ptime_t currentTime
                                  = time_types::ptime_t()) = 0;

  /**
   * @brief Returns Model's snapshot after last computation.
   * @return Snapshot of Model after last computation.
   */
  virtual Snapshot getSnapshot() const = 0;

  /**
   * @brief Returns world static map.
   * @return MapPtr pointing to world Map.
   */
  virtual MapPtr getMap() const = 0;
};

} // namespace Model

#endif // MODEL_H
