// TODO implement this

#include <memory>

#include <Common/blockingqueue.hpp>
#include <Controller/common/message.h>

namespace Controller
{

class MainController
{
public:
  MainController(std::shared_ptr<Common::BlockingQueue<Controller::Message*> >);

private:
  std::shared_ptr<Common::BlockingQueue<Controller::Message*> > blockingQueue_;
};

} // namespace Controller
