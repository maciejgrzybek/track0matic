#include "timersmanager.h"

namespace Common
{

TimersManager::TimersManager(std::size_t threadPoolSize)
  : threadPoolSize_(threadPoolSize)
{}

std::size_t TimersManager::getThreadPoolSize() const
{
  return threadPoolSize_;
}

void TimersManager::setThreadPoolSize(std::size_t threadPoolSize)
{
  if (threadPoolSize < threadPoolSize_)
  {
    // FIXME implement this
    // release threads above pool size
  }
  threadPoolSize_ = threadPoolSize;
}

void TimersManager::startTimer(std::size_t interval,
                               std::shared_ptr<Callable> callable)
{
  // boost instead of std, because of compatibility with boost::asio
  boost::shared_ptr<EventTimer> timer(
          new EventTimer(io_service_,callable,interval)
        );
  timer->start();
  // EventTimer is already put (by EventTimer::start()) into boost::asio queue,
  // with shared_from_this(), so EventTimer will be held as long as needed.

  if (threadsGroup_.size() < threadPoolSize_)
  { // if not enough threads, create new
    std::atomic_bool* workIndicator = new std::atomic_bool();
    *workIndicator = true;
    Worker* worker = new Worker(io_service_,*workIndicator,timer);
    workers_.insert(std::make_pair(worker,workIndicator));
    threadsGroup_.create_thread(*worker);
  }
}

void TimersManager::stopTimers()
{
  for (std::pair<Worker*,std::atomic_bool*> worker : workers_)
  {
    *(worker.second) = false; // switch indicator to stop working
    worker.first->stop(); // cancels the timer
  }
  io_service_.stop(); // don't accept asio::run() invocation anymore
  threadsGroup_.join_all(); // wait for unfinished workers
}

Worker::Worker(boost::asio::io_service& io_service,
               const std::atomic_bool& working,
               boost::shared_ptr<EventTimer> eventTimer)
  : io_service_(io_service),
    working_(working),
    eventTimer_(eventTimer)
{}

void Worker::stop()
{
  eventTimer_->stop();
}

void Worker::operator()()
{
  while (working_) // if there is still work permission
  {
    boost::system::error_code ec; // to prevent throwing from io_service
    io_service_.run(ec);
  } // if something went wrong previously, rerun io_service (while loop).
}

} // namespace Common
