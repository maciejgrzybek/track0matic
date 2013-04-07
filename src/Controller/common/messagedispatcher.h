#ifndef CONTROLLER_MESSAGEDISPATCHER_H
#define CONTROLLER_MESSAGEDISPATCHER_H

#include <memory>

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

class Controller;

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
                    View::View&);

  void visit(class WorkingModeChangeMessage&);
  void visit(class TimerTickMessage&);

private:
  Model::Model& model_;
  Controller& controller_;
  View::View& view_;
};

} // namespace Controller

#endif // CONTROLLER_MESSAGEDISPATCHER_H

