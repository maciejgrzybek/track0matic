#include "maincontroller.h"

#include <Common/logger.h>

namespace Controller
{

MainController
::MainController(std::shared_ptr<Common::BlockingQueue<MessagePtr> > bq,
                 std::unique_ptr<Model::Model> model,
                 std::unique_ptr<View::View> view)
  : blockingQueue_(bq),
    model_(std::move(model)),
    view_(std::move(view)),
    messageDispatcher_(new MessageDispatcher(*model_,
                                             *this,
                                             *view_)),
    timersManager_(new Common::TimersManager()),
    working_(true)
{
  std::shared_ptr<Common::Callable> callable(
          new RefreshEventProducer(blockingQueue_)
        );
  timersManager_->startTimer(1000,callable);
}

MainController::~MainController()
{}

void MainController::operator()()
{
  Common::GlobalLogger& logger = Common::GlobalLogger::getInstance();
  logger.log("MainController","Entering main loop.");
  while (working_) // main loop
  {
    MessagePtr msg;
    blockingQueue_->pop(msg); // get message from queue or hang on,
                              // when no messages available
    // perform action appropriate for received msg
    msg->accept(*messageDispatcher_);
  }
  logger.log("MainController","Left main loop.");
  view_->quit(); // before stopping work, close View
}

void MainController::quit()
{
  working_ = false;
}

bool MainController::isWorking() const
{
  return working_;
}

RefreshEventProducer::RefreshEventProducer(std::shared_ptr<
                                            Common::BlockingQueue<
                                              MessagePtr>
                                           > blockingQueue)
  : blockingQueue_(blockingQueue)
{}

void RefreshEventProducer::operator()(Common::EventTimer* /*timer*/)
{
  MessagePtr msg(new TimerTickMessage(time_types::duration_t(1))); // 1 interval

  blockingQueue_->push(msg);
}

} // namespace Controller
