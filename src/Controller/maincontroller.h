#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <memory>

#include <Common/blockingqueue.hpp>

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
  MainController(std::shared_ptr<Common::BlockingQueue<Message*> >,
                 std::unique_ptr<Model::Model>,
                 std::unique_ptr<View::View>);
  virtual ~MainController();

  virtual void operator()();

private:
  std::unique_ptr<Model::Model> model_;
  std::unique_ptr<View::View> view_;
  std::shared_ptr<Common::BlockingQueue<Message*> > blockingQueue_;
  const std::unique_ptr<MessageDispatcher> messageDispatcher_;
};

} // namespace Controller

#endif // MAINCONTROLLER_H
