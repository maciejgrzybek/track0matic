#ifndef QTVIEW_H
#define QTVIEW_H

#include <View/view.h>

namespace View
{

class QtView : public View
{
public:
  virtual void showState(Model::Snapshot);
};

} // namespace View

#endif // QTVIEW_H
