// TODO implement this

#include "maincontroller.h"

namespace Controller
{

MainController
::MainController(std::shared_ptr<Common::BlockingQueue<View::Message*> > bq)
  : blockingQueue_(bq)
{}

} // namespace Controller

