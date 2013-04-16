#include "qtview.h"

#include <Common/logger.h>

#include <Controller/common/message.h>

#include <View/Graphic/qtrenderer.h>

namespace View
{

namespace Graphic
{

QtView
::QtView(std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >bq)
  : blockingQueue_(bq),
    renderer_(new QtRenderer(360,360,this)) // TODO parametrize this
{
  renderer_->show();
}

QtView::~QtView()
{
  delete renderer_;
}

void QtView::showState(Model::Snapshot snapshot)
{
  std::shared_ptr<std::set<std::unique_ptr<Track> > > tracks
      = snapshot.getData();

  for (const std::unique_ptr<Track>& track : *tracks)
  {
    renderer_->addTrack(&*track); // ugly &*,
                // but std::unique_ptr::release() not allowed in const object,
                // std::set implices const
  }
}

void QtView::quitRequested()
{
  Controller::MessagePtr msg(new Controller::QuitRequestedMessage());
  blockingQueue_->push(msg);
}

void QtView::quit()
{
  Common::GlobalLogger::getInstance().log("QtView","Quit requested.");
  renderer_->close();
}

} // namespace Graphic

} // namespace View

