#include <memory>

#include <Common/blockingqueue.hpp>

#include <Model/datamanager.h>

#include <Controller/maincontroller.h>
#include <Controller/common/message.h>

#include <View/qtview.h>

int main(void)
{
  std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >
    blockingQueue(new Common::BlockingQueue<Controller::MessagePtr>());

  std::unique_ptr<Model::Model> m(new Model::DataManager());
  std::unique_ptr<View::View> v(new View::QtView(blockingQueue));
  std::unique_ptr<Controller::Controller> c(
        new Controller::MainController(blockingQueue,std::move(m),std::move(v))
        );

  c->operator()();

  return 0;
}

