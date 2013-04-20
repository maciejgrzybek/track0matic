#include <algorithm> // for std::transform
#include <iostream> // for program options' output
#include <memory>
#include <string>

#include <boost/program_options.hpp>

#include <QApplication>
#include <QThread>

#include <Common/blockingqueue.hpp>
#include <Common/logger.h>

#include <Model/datamanager.h>

#include <Controller/controller.h> // for enum WorkMode

#include <Controller/maincontroller.h>
#include <Controller/qtcontrollerwrapper.h>
#include <Controller/common/message.h>

#include <View/qtview.h>

namespace bpo = boost::program_options;

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

struct ProgramOptions
{
  ProgramOptions()
    : workMode(Controller::Batch)
  {}

  ProgramOptions(Controller::WorkMode workMode)
    : workMode(workMode)
  {}

  std::string toString() const
  {
    std::stringstream result;
    std::string mode;
    switch (workMode)
    {
      case Controller::Batch:
        mode = "batch";
        break;
      case Controller::Online:
        mode = "online";
        break;
      default:
        assert(false && "Not implemented WorkMode!");
        break;
    }

    result << "mode = " << mode;
    return result.str();
  }

  Controller::WorkMode workMode;
};

ProgramOptions prepareProgramOptions(int argc, char* argv[]);

int main(int argc, char* argv[])
{
  Common::GlobalLogger& logger = Common::GlobalLogger::getInstance();
  logger.setAgent(std::unique_ptr<Common::LoggerAgent>(
                    new Common::ConsoleLoggerAgent()));
  logger.log("main","Logger setup complete.");

  ProgramOptions po;
  try
  {
    po = prepareProgramOptions(argc,argv);
  }
  catch (const HelpRequestedException&)
  {
    return 1;
  }

  std::stringstream msg;
  msg << "Choosen options: " << po.toString();
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
                                        std::move(view),
                                        po.workMode)
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

std::istream& operator>>(std::istream& in, Controller::WorkMode& mode)
{
    std::string token;
    in >> token;
    std::transform(token.begin(), token.end(), token.begin(), ::tolower);
    if (token == "batch")
      mode = Controller::Batch;
    else if (token == "online")
      mode = Controller::Online;
    else
      throw bpo::validation_error(
          bpo::validation_error::invalid_option_value,"mode",token);

    return in;
}

ProgramOptions prepareProgramOptions(int argc, char* argv[])
{
  ProgramOptions po;

  bpo::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "message you're reading")
      ("mode,m", bpo::value<Controller::WorkMode>(),
                 "work mode - Online or Batch")
  ;
  bpo::variables_map vm;
  try
  {
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);
  }
  catch (const boost::program_options::unknown_option&)
  {
    std::cout << "Unrecognized option!" << std::endl;
    std::cout << desc << std::endl;
    throw HelpRequestedException();
  }

  if (vm.count("help") > 0)
  {
    std::cout << desc << std::endl;
    throw HelpRequestedException();
  }

  if (vm.count("mode") > 0)
  {
    po.workMode = vm["mode"].as<Controller::WorkMode>();
  } // if mode is not given,
    // default constructed ProgramOptions chooses default work mode

  return po;
}
