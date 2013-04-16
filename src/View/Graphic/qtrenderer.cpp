#include "qtrenderer.h"

#include <QMainWindow>
#include <QMenuBar>

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QColor>
#include <QPen>

#include <Model/track.h>

namespace View
{

namespace Graphic
{

GraphicalTrack::GraphicalTrack(boost::uuids::uuid uuid,
                               qreal x, qreal y,
                               qreal width, qreal height)
  : QGraphicsEllipseItem(x,y,width,height),
    uuid_(uuid)
{}

/******************************************************************************/

QtRenderer::QtRenderer(std::size_t width, std::size_t height, QObject* parent)
  : QObject(parent),
    mainWindow_(new QMainWindow()),
    scene_(new QGraphicsScene(-qreal(width/2.0),-qreal(height/2.0),width,height)),
    view_(new QGraphicsView(scene_))
{
  connect(this,SIGNAL(addTrackSignal(GraphicalTrack*)),
          SLOT(performAddTrack(GraphicalTrack*)));

  connect(this,SIGNAL(clearSceneSignal()),scene_,SLOT(clear()));

  drawStaticGraphics();
  setupMenu();
  view_->scale(2.0,2.0);
  mainWindow_->setCentralWidget(view_);
}

QtRenderer::~QtRenderer()
{
  delete view_;
  delete scene_;
  delete mainWindow_;
}

void QtRenderer::show()
{
  mainWindow_->show();
}

void QtRenderer::addTrack(const Track* track)
{
  GraphicalTrack* graphicalTrack = transformTrackFromSnapshot(track);
  emit addTrackSignal(graphicalTrack); // invokeLater (put into Qt msg queue)
}

void QtRenderer::clearScene()
{
  emit clearSceneSignal(); // invokeLater
}

void QtRenderer::performAddTrack(GraphicalTrack* graphicalTrack)
{
  scene_->addItem(graphicalTrack);
}

void QtRenderer::quit()
{
  mainWindow_->close();
}

GraphicalTrack* QtRenderer::transformTrackFromSnapshot(const Track* track)
{
  qreal x = track->getLongitude();
  qreal y = track->getLatitude();

  GraphicalTrack* graphicalTrack = new GraphicalTrack(track->getUuid(),x,y);
  return graphicalTrack;
}

void QtRenderer::drawStaticGraphics()
{
  drawBackground();
}

void QtRenderer::drawBackground()
{
  QPen pen(QColor(0,0,0));
  pen.setWidth(1);
  QRectF rect = scene_->sceneRect();
  qreal midWidth = rect.left() + rect.width()/2;
  qreal midHeight = rect.top() + rect.height()/2;
  scene_->addLine(midWidth,rect.top(),midWidth,rect.bottom(),pen); // horizontal line
  scene_->addLine(rect.left(),midHeight,rect.right(),midHeight,pen); // vertical line
}

void QtRenderer::setupMenu()
{
  mainWindow_->menuBar()->addAction(tr("Quit"), this, SLOT(quit()));
}

} // namespace Graphic

} // namespace View
