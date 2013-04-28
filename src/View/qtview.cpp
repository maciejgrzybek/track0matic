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
    renderer_(new QtRenderer(this)) // TODO parametrize this
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

  {
    std::stringstream msg;
    msg << "Received snapshot, containing " << tracks->size() << " track(s).";
    Common::GlobalLogger::getInstance().log("QtView",msg.str());
  }
  renderer_->clearScene(); // TODO consider not clearing scene (background)

  for (const std::unique_ptr<Track>& track : *tracks)
  {
    renderer_->addTrack(&*track); // ugly &*,
                // but std::unique_ptr::release() not allowed in const object,
                // std::set implices const

    // This is safe (no race conditions),
    //  because QtRenderer transforms this into it's own object, than returns,
    //  so needed object is hold as long as needed.
  }
}

void QtView::worldStateChange(std::unique_ptr<Model::WorldSnapshot> snapshot)
{
  {
    std::stringstream msg;
    msg << "Received world snapshot, containing " << snapshot->edges.size()
        << " edges and " << snapshot->vertexes.size() << " vertexes.";
    Common::GlobalLogger::getInstance().log("QtView",msg.str());
  }

  for (const std::shared_ptr<Model::WorldSnapshot::StreetSnapshot> street
        : snapshot->edges)
  {
    renderer_->addStreet(street);
  }
}

void QtView::quitRequested()
{
  Controller::MessagePtr msg(new Controller::QuitRequestedMessage());
  blockingQueue_->push(msg);
}

void QtView::requestMapData()
{
  Controller::MessagePtr msg(new Controller::MapSnapshotRequestedMessage());
  blockingQueue_->push(msg);
}

void QtView::quit()
{
  Common::GlobalLogger::getInstance().log("QtView","Quit requested.");
  renderer_->close();
}

} // namespace Graphic

} // namespace View

