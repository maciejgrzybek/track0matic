#ifndef THREADBUFFER_HPP
#define THREADBUFFER_HPP

#include <boost/thread/shared_mutex.hpp>

namespace Common
{

template <class Type>
class ThreadBuffer
{
public:
  Type get() const
  {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    return item_;
  }

  void put(const Type& item)
  {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    item_ = item;
  }

private:
  Type item_;
  mutable boost::shared_mutex mutex_;
};

} // namespace Common

#endif // THREADBUFFER_HPP
