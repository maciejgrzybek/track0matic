// TODO implement this

#include <memory>

#include <Common/blockingqueue.hpp>
#include <Controller/controller.h>
#include <Controller/common/message.h>

#include <Model/datamanager.h>

#include <View/view.h>

namespace Controller
{

class MainController : public Controller
{
public:
  MainController(std::shared_ptr<Common::BlockingQueue<Message*> >,
                 std::unique_ptr<DataManager>,
                 std::unique_ptr<View::View>);
  virtual ~MainController();

  virtual void operator()();

private:
  std::unique_ptr<DataManager> model_;
  std::unique_ptr<View::View> view_;
  std::shared_ptr<Common::BlockingQueue<Message*> > blockingQueue_;
};

} // namespace Controller
