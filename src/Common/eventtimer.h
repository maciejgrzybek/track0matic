#ifndef EVENTTIMER_H
#define EVENTTIMER_H

#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace Common
{

class EventTimer;

class Callable : public std::unary_function<EventTimer*,void>
{
public:
  virtual void operator()(EventTimer*) = 0;
};

class EventTimer : public boost::enable_shared_from_this<EventTimer>
{
public:
  EventTimer(boost::asio::io_service&,
             std::shared_ptr<Callable>,
             std::size_t interval = 1000,
             bool repetitively = true);
  virtual ~EventTimer();

  void start();
  void stop();

  void setInterval(std::size_t milliseconds);

private:
  void setupTimer();
  void handleTimerEvent(const boost::system::error_code& error);

  boost::asio::io_service& io_service_;
  boost::asio::deadline_timer timer_;
  std::shared_ptr<Callable> callable_;
  std::size_t interval_;
  bool repetitively_;
};

} // namespace Common

#endif // EVENTTIMER_H
