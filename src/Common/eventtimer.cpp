#include "eventtimer.h"

#include <boost/bind.hpp>

namespace Common
{

EventTimer::EventTimer(boost::asio::io_service& io_service,
                       std::shared_ptr<Callable> callable,
                       std::size_t interval,
                       bool repetitively)
  : io_service_(io_service),
    timer_(io_service),
    callable_(callable),
    interval_(interval),
    repetitively_(repetitively)
{}

EventTimer::~EventTimer()
{}

void EventTimer::start()
{
  timer_.cancel();
  setupTimer();
}

void EventTimer::stop()
{
  timer_.cancel();
}

void EventTimer::setInterval(std::size_t milliseconds)
{
  interval_ = milliseconds;
}

void EventTimer::setupTimer()
{
  if (interval_ > 0)
  {
    timer_.expires_from_now(boost::posix_time::milliseconds(interval_));
    timer_.async_wait(
       boost::bind(&EventTimer::handleTimerEvent,
                   shared_from_this(),
                   boost::asio::placeholders::error));
  }
}

void EventTimer::handleTimerEvent(const boost::system::error_code& error)
{
  if (!error && interval_ > 0)
  {
    (*callable_)(this);
    if (repetitively_) // if timer is not single shot type
      setupTimer(); // setup next trigger
  }
}

} // namespace Common
