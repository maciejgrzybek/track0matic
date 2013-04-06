#ifndef CONTROLLER_H
#define CONTROLLER_H

namespace Controller
{

class Controller
{
public:
  virtual ~Controller()
  {}

  /**
   * @brief Starts the operating.
   *  Receives messages from from-view-queue and performes actions
   *  based on these messages.
   */
  virtual void operator()() = 0;
};

} // namespace Controller

#endif // CONTROLLER_H
