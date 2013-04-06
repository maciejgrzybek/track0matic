#include <memory>

#include <Common/blockingqueue.hpp>

#include <Model/datamanager.h>

#include <Controller/maincontroller.h>

#include <View/view.h>
#include <View/common/message.h>

int main(void)
{
  std::shared_ptr<Common::BlockingQueue<Controller::Message*> >
    blockingQueue(new Common::BlockingQueue<Controller::Message*>());

  DataManager dataManager;

  std::unique_ptr<View::View> v(new View::QtView());
  std::unique_ptr<Controller::Controller> c(
        new Controller::MainController(blockingQueue)
        );

  c->go();

  return 0;
}

