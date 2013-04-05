#include <memory>

#include <Common/blockingqueue.hpp>

#include <Model/datamanager.h>

#include <Controller/maincontroller.h>

#include <View/view.h>
#include <View/common/message.h>

int main(void)
{
  std::shared_ptr<Common::BlockingQueue<View::Message*> >
    blockingQueue(new Common::BlockingQueue<View::Message*>());

  DataManager dataManager();

  std::unique_ptr<View::QtView> v(new View::QtView());
  std::unique_ptr<Controller::MainController> c(
        new Controller::MainController(blockingQueue)
        );
  // TODO create view and connect with Controller
  return 0;
}

