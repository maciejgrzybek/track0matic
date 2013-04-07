#include "qtview.h"

namespace View
{

QtView
::QtView(std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >bq)
  : blockingQueue_(bq),
    timersManager_(new Common::TimersManager())
{
  std::shared_ptr<Common::Callable> callable(
          new RefreshEventProducer(blockingQueue_)
        );
  timersManager_->startTimer(1000,callable);
}

void QtView::showState(Model::Snapshot snapshot)
{
  // FIXME implement this
  std::shared_ptr<std::set<std::unique_ptr<Track> > > tracks = snapshot.getData();
}


RefreshEventProducer::RefreshEventProducer(std::shared_ptr<
                                            Common::BlockingQueue<
                                              Controller::MessagePtr>
                                           > blockingQueue)
  : blockingQueue_(blockingQueue)
{}

void RefreshEventProducer::operator()(Common::EventTimer* /*timer*/)
{
  Controller::MessagePtr msg(
          new Controller::TimerTickMessage(
            time_types::duration_t(1) // 1s interval
          )
        );

  blockingQueue_->push(msg);
}

} // namespace View
