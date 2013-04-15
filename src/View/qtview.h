#ifndef QTVIEW_H
#define QTVIEW_H

#include <memory>

#include <View/view.h>

#include <Common/blockingqueue.hpp>

#include <Controller/common/message.h>

class QApplication;

namespace View
{

namespace Graphic
{

class QtRenderer;

class QtView : public View
{
public:
  QtView(std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >);
  virtual void showState(Model::Snapshot);

private:
  std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> > blockingQueue_;
  QtRenderer* renderer_;
};

} // namespace Graphic

} // namespace View

#endif // QTVIEW_H
