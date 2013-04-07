#ifndef QTVIEW_H
#define QTVIEW_H

#include <memory>

#include <View/view.h>

#include <Common/blockingqueue.hpp>
#include <Common/eventtimer.h>
#include <Common/timersmanager.h>

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
  const std::unique_ptr<Common::TimersManager> timersManager_;
};

class RefreshEventProducer : public Common::Callable
{
public:
  RefreshEventProducer(std::shared_ptr<
                        Common::BlockingQueue<
                          Controller::MessagePtr>
                       > blockingQueue);
  virtual void operator()(Common::EventTimer*);

private:
  std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> > blockingQueue_;
};

} // namespace View

#endif // QTVIEW_H
