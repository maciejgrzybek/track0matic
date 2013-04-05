// TODO implement this

#include <memory>

#include <Common/blockingqueue.hpp>

#include <View/common/message.h>

namespace Controller
{

class MainController
{
public:
  MainController(std::shared_ptr<Common::BlockingQueue<View::Message*> >);

private:
  std::shared_ptr<Common::BlockingQueue<View::Message*> > blockingQueue_;
};

} // namespace Controller
