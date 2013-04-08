#ifndef QTVIEW_H
#define QTVIEW_H

#include <memory>

#include <View/view.h>

#include <Common/blockingqueue.hpp>

#include <Controller/common/message.h>

namespace View
{

class QtView : public View
{
public:
  QtView(std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >);
  virtual void showState(Model::Snapshot);

private:
  std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> > blockingQueue_;
};

} // namespace View

#endif // QTVIEW_H
