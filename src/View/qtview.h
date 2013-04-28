#ifndef QTVIEW_H
#define QTVIEW_H

#include <memory>

#include <View/view.h>

#include <Common/blockingqueue.hpp>

#include <Controller/common/message.h>

namespace View
{

namespace Graphic
{

class QtRenderer;

class QtView : public View
{
public:
  QtView(std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >);
  virtual ~QtView();
  virtual void showState(Model::Snapshot);
  virtual void worldStateChange(std::unique_ptr<Model::WorldSnapshot>);

  // invoked by renderer
  virtual void quitRequested();

  // invoked by renderer
  virtual void requestMapData();

  // invoked by Controller
  virtual void quit();

private:
  std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> > blockingQueue_;
  QtRenderer* renderer_;
};

} // namespace Graphic

} // namespace View

#endif // QTVIEW_H
