#ifndef CONTROLLER_MESSAGE_H
#define CONTROLLER_MESSAGE_H

#include <memory>

#include <Common/time.h>

namespace Controller
{

class Message;
class MessageDispatcher;
typedef std::shared_ptr<Message> MessagePtr;

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

class QuitRequestedMessage : public Message
{
public:
  virtual void accept(MessageDispatcher&);
};

class MapSnapshotRequestedMessage : public Message
{
public:
  virtual void accept(MessageDispatcher&);
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

