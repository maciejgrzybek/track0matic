#include "qtcontrollerwrapper.h"

#include <QApplication>

QtControllerWrapper
::QtControllerWrapper(std::unique_ptr<Controller::Controller> controller,
                      QObject* parent)
  : QObject(parent),
    controller_(std::move(controller))
{}

void QtControllerWrapper::startController()
{
  controller_->operator()();
  emit quit();
}

/******************************************************************************/

QtThreadManager::QtThreadManager(QtControllerWrapper* wrapper)
  : wrapper_(wrapper)
{}

void QtThreadManager::startThread()
{
  QThread* controllerThread = new QThread(this);
  connect(controllerThread,SIGNAL(started()),
          wrapper_,SLOT(startController()));
  connect(wrapper_,SIGNAL(quit()),controllerThread,SLOT(quit()));
  connect(controllerThread,SIGNAL(finished()),
          QApplication::instance(),SLOT(quit()));

  wrapper_->moveToThread(controllerThread);
  controllerThread->start();
}
