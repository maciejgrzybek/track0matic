#include "qtview.h"

namespace View
{

QtView
::QtView(std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >bq)
  : blockingQueue_(bq)
{}

void QtView::showState(Model::Snapshot snapshot)
{
  // FIXME implement this
  std::shared_ptr<std::set<std::unique_ptr<Track> > > tracks
      = snapshot.getData();
}

} // namespace View
