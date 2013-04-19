#ifndef CONTROLLER_MESSAGEDISPATCHER_H
#define CONTROLLER_MESSAGEDISPATCHER_H

#include <memory>

#include <Controller/controller.h> // for enum WorkMode;

namespace Model
{
class Model;
}

namespace View
{
class View;
}

namespace Controller
{

/**
 * @brief The MessageDispatcher class is responsible for doing actions,
 *  based on messaged arrived from message queue.
 */
class MessageDispatcher
{
public:
  // ensure Model, Controller and View are valid,
  //  until MessageDispatcher is valid
  MessageDispatcher(Model::Model&,
                    Controller&,
                    View::View&,
                    WorkMode);

  void visit(class WorkingModeChangeMessage&);
  void visit(class QuitRequestedMessage&);
  void visit(class TimerTickMessage&);

private:
  Model::Model& model_;
  Controller& controller_;
  View::View& view_;

  WorkMode workMode_;
};

} // namespace Controller

#endif // CONTROLLER_MESSAGEDISPATCHER_H

