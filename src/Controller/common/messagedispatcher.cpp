#include "messagedispatcher.h"

#include <cassert>

#include <Common/configurationmanager.h>

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

void MessageDispatcher::visit(MapSnapshotRequestedMessage& /*m*/)
{
  std::unique_ptr<Model::WorldSnapshot> snapshot(
        new Model::WorldSnapshot(model_.getMap())
        );
  view_.worldStateChange(std::move(snapshot));
}

void MessageDispatcher::visit(TimerTickMessage& /*m*/)
{
  Model::Snapshot state;

  Common::Configuration::ConfigurationManager& confMan
      = Common::Configuration::ConfigurationManager::getInstance();

  const std::string workMode = confMan["Controller"]["WorkMode"];

  if (workMode == "online")
    state = model_.computeState(time_types::clock_t::now());
  else if (workMode == "batch")
    state = model_.computeState();
  else
    assert(false && "Not implemented WorkMode!");

  view_.showState(state); // put snapshot to View
}

} // namespace Controller
