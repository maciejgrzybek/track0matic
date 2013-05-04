#include <algorithm> // for std::transform
#include <iostream> // for program options' output
#include <memory>
#include <string>

#include <QApplication>
#include <QThread>

#include <Common/blockingqueue.hpp>
#include <Common/configurationmanager.h>
#include <Common/logger.h>

#include <Model/datamanager.h>

#include <Controller/controller.h> // for enum WorkMode

#include <Controller/maincontroller.h>
#include <Controller/qtcontrollerwrapper.h>
#include <Controller/common/message.h>

#include <View/qtview.h>

class HelpRequestedException : public std::exception
{
public:
  virtual ~HelpRequestedException() throw()
  {}

  virtual const char* what() const throw()
  {
    return "Help requested in command line arguments.";
  }
};

int main(int argc, char* argv[])
{
  Common::GlobalLogger& logger = Common::GlobalLogger::getInstance();
  logger.setAgent(std::unique_ptr<Common::LoggerAgent>(
                    new Common::ConsoleLoggerAgent()));
  logger.log("main","Logger setup complete.");

  Common::Configuration::ConfigurationManager::KeyValueMap options
      = Common::Configuration::getConfigurationFromFile("settings.ini");
  Common::Configuration::ConfigurationManager& confMan
      = Common::Configuration::ConfigurationManager::getInstance();
  confMan.parseKeyValueMapIntoConfiguration(options);

  std::stringstream msg;
  msg << "Choosen options: \n" << confMan.toString();
  logger.log("main",msg.str());

  QApplication app(argc,argv);

  std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >
    blockingQueue(new Common::BlockingQueue<Controller::MessagePtr>());

  std::unique_ptr<Model::Model> model(new Model::DataManager());
  logger.log("main","Model instantiated.");
  std::unique_ptr<View::View> view(new View::Graphic::QtView(blockingQueue));
  logger.log("main","View instantiated.");
  std::unique_ptr<Controller::Controller> controller(
         new Controller::MainController(blockingQueue,
                                        std::move(model),
                                        std::move(view))
        );
  logger.log("main","Controller instantiated.");

  /* <ugly hack> */
  // thread is needed to prevent main thread from blocking,
  //  allowing reach app.exec()
  QtControllerWrapper* controllerWrapper
      = new QtControllerWrapper(std::move(controller));
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
