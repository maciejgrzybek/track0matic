// TODO implement this

#include "maincontroller.h"

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
                                             *view_))
{}

MainController::~MainController()
{}

void MainController::operator()()
{
  while (true) // main loop
  {
    MessagePtr msg;
    blockingQueue_->pop(msg); // get message from queue or hang on,
                              // when no messages available
    // perform action appropriate for received msg
    msg->accept(*messageDispatcher_);
  }
}

} // namespace Controller
