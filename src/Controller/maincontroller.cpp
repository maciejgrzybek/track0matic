// TODO implement this

#include "maincontroller.h"

namespace Controller
{

MainController
::MainController(std::shared_ptr<Common::BlockingQueue<Message*> > bq,
                 std::unique_ptr<Model::Model> model,
                 std::unique_ptr<View::View> view)
  : blockingQueue_(bq),
    model_(std::move(model)),
    view_(std::move(view))
{}

MainController::~MainController()
{}

void MainController::operator()()
{
  while (true) // main loop
  {
    // TODO get messages from View here
    Model::Snapshot state = model_->computeState(); // get data from Model
    view_->showState(state); // put snapshot to View
  }
}

} // namespace Controller

