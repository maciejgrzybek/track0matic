#ifndef MODEL_H
#define MODEL_H

#include <Model/modelsnapshot.h>

namespace Model
{

class Model
{
public:
  virtual ~Model()
  {}

  /**
   * @brief Computes next state of Model.
   * @return Snapshot of Model after computation.
   */
  virtual Snapshot computeState() = 0;

  /**
   * @brief Returns Model's snapshot after last computation.
   * @return Snapshot of Model after last computation.
   */
  virtual Snapshot getSnapshot() const = 0;
};

} // namespace Model

#endif // MODEL_H
