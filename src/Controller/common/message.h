#ifndef CONTROLLER_MESSAGE_H
#define CONTROLLER_MESSAGE_H

#include <Common/time.h>

namespace Controller
{

class MessageDispatcher;

class Message
{
public:
  virtual void accept(MessageDispatcher&) = 0;

protected:
  Message() = default;
  virtual ~Message();
};

class WorkingModeChangeMessage : public Message
{
public:
  enum WorkingMode { BATCH, LIVE };

  WorkingModeChangeMessage(WorkingMode workingMode);
  WorkingMode getMode() const;
  virtual void accept(MessageDispatcher&);

private:
  WorkingMode workingMode_;
};

class TimerTickMessage : public Message
{
public:
  TimerTickMessage(time_types::duration_t timeDuration);
  virtual void accept(MessageDispatcher&);

private:
  time_types::duration_t timeDuration_;
};

} // namespace Controller

#endif // Controller_MESSAGE_H

