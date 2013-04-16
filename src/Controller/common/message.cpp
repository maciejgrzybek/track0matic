#include "message.h"

#include "messagedispatcher.h"

namespace Controller
{

Message::~Message()
{}

WorkingModeChangeMessage::WorkingModeChangeMessage(WorkingMode workingMode)
  : workingMode_(workingMode)
{}

WorkingModeChangeMessage::WorkingMode WorkingModeChangeMessage::getMode() const
{
  return workingMode_;
}

void WorkingModeChangeMessage::accept(MessageDispatcher& md)
{
  md.visit(*this);
}

void QuitRequestedMessage::accept(MessageDispatcher& md)
{
  md.visit(*this);
}

TimerTickMessage::TimerTickMessage(time_types::duration_t timeDuration)
  : timeDuration_(timeDuration)
{}

void TimerTickMessage::accept(MessageDispatcher& md)
{
  md.visit(*this);
}

} // namespace Controller
