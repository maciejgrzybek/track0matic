#include "qtrenderer.h"

#include <cassert>

#include <boost/random/uniform_int_distribution.hpp>

#include <QMainWindow>
#include <QMenuBar>

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QColor>
#include <QPen>

#include <View/qtview.h>

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

boost::uuids::uuid GraphicalTrack::getUuid() const
{
  return uuid_;
}

void GraphicalTrack::moveBasedOnOther(const GraphicalTrack* track)
{
  update(track->boundingRect());
}

/******************************************************************************/

QtRenderer::QtRenderer(std::size_t width, std::size_t height, QtView* parent)
  : parent_(parent),
    mainWindow_(new QMainWindow()),
    scene_(new QGraphicsScene(-qreal(width/2.0),-qreal(height/2.0),width,height)),
    view_(new QGraphicsView(scene_))
{
  mainWindow_->setAttribute(Qt::WA_QuitOnClose,false);
  connect(this,SIGNAL(addTrackSignal(GraphicalTrack*)),
          SLOT(performAddTrack(GraphicalTrack*)));

  connect(this,SIGNAL(clearSceneSignal()),scene_,SLOT(clear()));

  connect(this,SIGNAL(exitRequestedSignal()),SLOT(quitRequested()));

  connect(this,SIGNAL(showSignal()),mainWindow_,SLOT(show()));
  connect(this,SIGNAL(quitSignal()),mainWindow_,SLOT(close()));

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
  emit showSignal();
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

void QtRenderer::requestExit()
{
  emit exitRequestedSignal(); // invokeLater
}

void QtRenderer::close()
{
  emit quitSignal();
}

void QtRenderer::performAddTrack(GraphicalTrack* graphicalTrack)
{
  // rewrite it to be done in cleaner way
  boost::uuids::uuid uuid = graphicalTrack->getUuid();
  QMap<boost::uuids::uuid,GraphicalTrack*>::const_iterator iter
      = tracks_.find(uuid);
  GraphicalTrack* current = nullptr;
  if (iter == tracks_.end())
  { // track didn't exist in Scene
    scene_->addItem(graphicalTrack);
    tracks_[uuid] = graphicalTrack;
    current = graphicalTrack;
  }
  else
  { // track already existed in Scene
    iter.value()->moveBasedOnOther(graphicalTrack);
    current = iter.value();
  }

  chooseColorForTrack(current);
}

void QtRenderer::quitRequested()
{
  assert(parent_);
  parent_->quitRequested();
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
  mainWindow_->menuBar()->addAction(tr("Quit"), this, SLOT(quitRequested()));
}

void QtRenderer::chooseColorForTrack(GraphicalTrack* track)
{
  if (track->brush() != QBrush()) // brush already set
    return; // do not change

  QPen pen(Qt::yellow); // yellow pen (border)
  std::vector<QColor> colors = {
                                 Qt::red,
                                 Qt::green,
                                 Qt::blue
                               };
  boost::random::uniform_int_distribution<> dist(0, colors.size()-1);
  std::size_t num = dist(randomGenerator_);
  QColor color = colors.at(num);
  QBrush brush(color);
  track->setPen(pen);
  track->setBrush(brush);
}

} // namespace Graphic

} // namespace View
