#ifndef CONTROLLER_MESSAGE_H
#define CONTROLLER_MESSAGE_H

#include <Common/time.h>

namespace Controller
{

class Message
{
protected:
  Message() = default;
};

class WorkingModeChangeMessage : public Message
{
public:
  enum WorkingMode { BATCH, LIVE };

  WorkingModeChangeMessage(WorkingMode workingMode);
  WorkingMode getMode() const;

private:
  WorkingMode workingMode_;
};

class TimerTickMessage : public Message
{
public:
private:

};

} // namespace Controller

#endif // Controller_MESSAGE_H

