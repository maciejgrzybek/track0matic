#ifndef VIEW_H
#define VIEW_H

#include <Model/modelsnapshot.h>

namespace View
{

class View
{
public:
  ~View()
  {}

  virtual void showState(Model::Snapshot) = 0;
};

} // namespace View

#endif // VIEW_H
