#include <memory>

#include <QApplication>
#include <QThread>

#include <Common/blockingqueue.hpp>
#include <Common/logger.h>

#include <Model/datamanager.h>

#include <Controller/maincontroller.h>
#include <Controller/qtcontrollerwrapper.h>
#include <Controller/common/message.h>

#include <View/qtview.h>

int main(int argc, char* argv[])
{
  Common::GlobalLogger& logger = Common::GlobalLogger::getInstance();
  logger.setAgent(std::unique_ptr<Common::LoggerAgent>(
                    new Common::ConsoleLoggerAgent()));
  logger.log("main","Logger setup complete");
  QApplication app(argc,argv);

  std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >
    blockingQueue(new Common::BlockingQueue<Controller::MessagePtr>());

  std::unique_ptr<Model::Model> m(new Model::DataManager());
  logger.log("main","Model instantiated.");
  std::unique_ptr<View::View> v(new View::Graphic::QtView(blockingQueue));
  logger.log("main","View instantiated.");
  std::unique_ptr<Controller::Controller> c(
         new Controller::MainController(blockingQueue,std::move(m),std::move(v))
        );
  logger.log("main","Controller instantiated.");

  /* <ugly hack> */
  // thread is needed to prevent main thread from blocking,
  //  allowing reach app.exec()
  QtControllerWrapper* controllerWrapper
      = new QtControllerWrapper(std::move(c));
  std::unique_ptr<QtThreadManager> threadManager(
        new QtThreadManager(controllerWrapper));
  threadManager->startThread();

  // Used wrapper for Controller in Qt, to use Qt-threads,
  //  which can be joined in QApplication.
  // I do not want to introduce Qt everywhere (especially to Controller,
  // nor Model), so QApplication is not passed to my components.
  // Because of this, special wrapper (QtControllerWrapper) has been written,
  //  to allow notification in Qt-way about finishing controller's thread
  /* </ugly hack> */

  return app.exec();
}

