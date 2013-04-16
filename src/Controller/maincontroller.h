#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <atomic>
#include <memory>

#include <Common/blockingqueue.hpp>
#include <Common/eventtimer.h>
#include <Common/timersmanager.h>

#include <Controller/controller.h>
#include <Controller/common/message.h>
#include <Controller/common/messagedispatcher.h>

#include <Model/datamanager.h>

#include <View/view.h>

namespace Controller
{

class MainController : public Controller
{
public:
  MainController(std::shared_ptr<Common::BlockingQueue<MessagePtr> >,
                 std::unique_ptr<Model::Model>,
                 std::unique_ptr<View::View>);
  virtual ~MainController();

  virtual void operator()();
  virtual void quit();
  bool isWorking() const;

private:
  std::unique_ptr<Model::Model> model_;
  std::unique_ptr<View::View> view_;
  std::shared_ptr<Common::BlockingQueue<MessagePtr> > blockingQueue_;
  const std::unique_ptr<MessageDispatcher> messageDispatcher_;
  const std::unique_ptr<Common::TimersManager> timersManager_;

  std::atomic_bool working_;
};

class RefreshEventProducer : public Common::Callable
{
public:
  RefreshEventProducer(std::shared_ptr<
                        Common::BlockingQueue<MessagePtr>
                       > blockingQueue);
  virtual void operator()(Common::EventTimer*);

private:
  std::shared_ptr<Common::BlockingQueue<MessagePtr> > blockingQueue_;
};

} // namespace Controller

#endif // MAINCONTROLLER_H
