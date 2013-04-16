#ifndef VIEW_H
#define VIEW_H

#include <Model/modelsnapshot.h>

namespace View
{

class View
{
public:
  virtual ~View()
  {}

  virtual void showState(Model::Snapshot) = 0;

  // closes visualization part of application
  virtual void quit() = 0;
};

} // namespace View

#endif // VIEW_H
