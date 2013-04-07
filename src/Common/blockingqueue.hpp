#ifndef BLOCKINGQUEUE_HPP
#define BLOCKINGQUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

namespace Common
{

class NoTimeoutWait
{
protected:
  template <class Predicate>
  void wait(std::condition_variable& cv,
            std::unique_lock<std::mutex>& l,
            Predicate pred)
  {
    cv.wait(l,pred);
  }
};

template <int Milliseconds>
class TimeoutWait
{
protected:
  template <class Predicate>
  void wait(std::condition_variable& cv,
            std::unique_lock<std::mutex>& l,
            Predicate pred)
  {
    cv.wait_for(l,std::chrono::milliseconds(Milliseconds),pred);
  }
};

template <class Type, class WaitingPolicy = NoTimeoutWait>
class BlockingQueue : private WaitingPolicy
{
public:
  /**
   * @brief Puts given element as a last in queue.
   * @param Element to put.
   *  Must be valid until method finishes it's work (returns).
   */
  void push(const Type& value)
  {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      queue_.push(value);
    }
    condVar_.notify_one();
  }

  /**
   * @brief Retrieves first element from queue.
   * @param Reference to object where to return an element. It has to be valid
   *  until method finishes it's work.
   *
   * @note Returns by reference instead of by value,
   * because Type's copy constructor (r-value here) does not have to be noexcept.
   * In such a case, when copy constructor of Type throws on std::move(),
   * the queue becomes unconsistent (element moved, by not pop_back()'ed).
   */
  void pop(Type& result)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    // wait until underlying container (queue) become non-empty
    this->wait(condVar_,lock,[this]{ return !queue_.empty(); });
    result = std::move(queue_.front());
    queue_.pop();
  }

private:
  std::condition_variable condVar_;
  std::mutex mutex_;
  std::queue<Type> queue_; // underlying container
};

} // namespace Common

#endif

