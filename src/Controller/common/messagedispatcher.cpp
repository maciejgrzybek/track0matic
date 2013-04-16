#include "messagedispatcher.h"

#include <Model/model.h>
#include <Model/modelsnapshot.h>

#include <Controller/controller.h>

#include <View/view.h>

#include "message.h"

namespace Controller
{

MessageDispatcher::MessageDispatcher(Model::Model& model,
                                     Controller& controller,
                                     View::View& view)
  : model_(model),
    controller_(controller),
    view_(view)
{}

void MessageDispatcher::visit(WorkingModeChangeMessage& /*m*/)
{
  // FIXME implement this
  // perform an action appropriate for WorkingModeChange message
}

void MessageDispatcher::visit(QuitRequestedMessage& /*m*/)
{
  controller_.quit();
}

void MessageDispatcher::visit(TimerTickMessage& /*m*/)
{
  Model::Snapshot state = model_.computeState(); // get data from Model
  view_.showState(state); // put snapshot to View
}

} // namespace Controller
