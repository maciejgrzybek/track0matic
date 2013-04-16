#ifndef QTRENDERERWRAPPER_H
#define QTRENDERERWRAPPER_H

#include <memory>

#include <QObject>
#include <QThread>

#include <Controller/controller.h>

class QtControllerWrapper : public QObject
{
  Q_OBJECT
public:
  explicit QtControllerWrapper(std::unique_ptr<Controller::Controller>,
                               QObject* parent = 0);

signals:
  void quit();

public slots:
  void startController();

private:
  std::unique_ptr<Controller::Controller> controller_;
};

/******************************************************************************/

class QtThreadManager : public QObject
{
  Q_OBJECT
public:
  explicit QtThreadManager(QtControllerWrapper* wrapper);
  void startThread();

private:
  QtControllerWrapper* const wrapper_;
};

#endif // QTRENDERERWRAPPER_H
