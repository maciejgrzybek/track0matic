#ifndef TIME_H
#define TIME_H

#include <boost/chrono.hpp> // internally includes chrono_io

// uses boost::chrono instead of std::chrono, because of IO lack in std
namespace time_types
{

typedef boost::chrono::system_clock clock_t;
typedef boost::chrono::time_point<clock_t> ptime_t;
typedef boost::chrono::duration<double> duration_t;
typedef boost::chrono::seconds seconds_t;

} // namespace time

#endif // TIME_H
