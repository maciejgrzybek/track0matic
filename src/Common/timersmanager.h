#ifndef TIMERSMANAGER_H
#define TIMERSMANAGER_H

#include <atomic>
#include <functional>
#include <unordered_map>

#include <boost/asio.hpp>
// uses boost::shared_ptr, because std's is not compatible with boost::asio
#include <boost/shared_ptr.hpp>
// uses boost::thread, because std does not have thread_group
#include <boost/thread/thread.hpp>

#include "eventtimer.h"

namespace Common
{

class Worker;

class TimersManager
{
public:
  TimersManager(std::size_t threadPoolSize = 1);
  std::size_t getThreadPoolSize() const;
  void setThreadPoolSize(std::size_t threadPoolSize);

  /**
   * @brief starts timer using internal io_service and thread pool
   * @param interval in milliseconds
   * @param callable - callback to invoke when timer fires
   */
  void startTimer(std::size_t interval, std::shared_ptr<Callable> callable);

  /**
   * @brief Stops all working timers.
   *  This won't fire any trigger (given callback for any timer).
   */
  void stopTimers();

private:
  boost::asio::io_service io_service_;
  boost::thread_group threadsGroup_;
  std::unordered_map<Worker*,std::atomic_bool*> workers_;
  std::size_t threadPoolSize_;
};

class Worker : public std::unary_function<void,void>
{
public:
  Worker(boost::asio::io_service& io_service,
         const std::atomic_bool& working,
         boost::shared_ptr<EventTimer> eventTimer);

  void stop(); // can be called from different thread
  void operator()();

private:
  boost::asio::io_service& io_service_;
  const std::atomic_bool& working_;
  boost::shared_ptr<EventTimer> eventTimer_;
};

} // namespace Common

#endif // TIMERSMANAGER_H
