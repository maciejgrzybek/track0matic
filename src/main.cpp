#include <memory>

#include <boost/thread.hpp>

#include <QApplication>

#include <Common/blockingqueue.hpp>

#include <Model/datamanager.h>

#include <Controller/maincontroller.h>
#include <Controller/common/message.h>

#include <View/qtview.h>

int main(int argc, char* argv[])
{
  QApplication app(argc,argv);

  std::shared_ptr<Common::BlockingQueue<Controller::MessagePtr> >
    blockingQueue(new Common::BlockingQueue<Controller::MessagePtr>());

  std::unique_ptr<Model::Model> m(new Model::DataManager());
  std::unique_ptr<View::View> v(new View::Graphic::QtView(blockingQueue));
  Controller::MainController c(blockingQueue,std::move(m),std::move(v));

  /* <ugly hack> */
  // thread is needed to prevent main thread from blocking,
  //  allowing reach app.exec()
  boost::thread controllerThread(boost::ref(c)); // ref allowed,
                                      // because scope of c is long enough
  // this thread is not joined properly, because I have to invoke app.exec(),
  // to have GUI in Qt working properly.
  // I do not want to introduce Qt everywhere (especially to Controller,
  // nor Model), so QApplication is not passed to my components.
  // Because of this, there is no way to clean exit application.
  /* </ugly hack> */

  return app.exec();
}

