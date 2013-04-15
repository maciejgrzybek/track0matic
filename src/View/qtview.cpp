#include "qtview.h"

#include <View/Graphic/qtrenderer.h>

namespace View
{

namespace Graphic
{

QtView
::QtView(std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >bq)
  : blockingQueue_(bq),
    renderer_(new QtRenderer(800,600)) // TODO parametrize this
{
  renderer_->show();
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

} // namespace Graphic

} // namespace View

