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
                               qreal predictionX, qreal predictionY,
                               qreal width, qreal height)
  : QGraphicsEllipseItem(x,y,width,height),
    prediction_(new QGraphicsEllipseItem(predictionX,predictionY,2*width,2*height)),
    uuid_(uuid)
{
  prediction_->setParentItem(this);
}

boost::uuids::uuid GraphicalTrack::getUuid() const
{
  return uuid_;
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

  connect(this,SIGNAL(clearSceneSignal()),SLOT(performClearScene()));

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
  emit quitSignal(); // invokeLater
}

void QtRenderer::performAddTrack(GraphicalTrack* graphicalTrack)
{
  colorManager_.setColorForTrack(graphicalTrack);
  scene_->addItem(graphicalTrack);
}

void QtRenderer::performClearScene()
{
  scene_->clear();
  drawStaticGraphics();
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
  qreal predictedX = track->getPredictedLongitude();
  qreal predictedY = track->getPredictedLatitude();

  GraphicalTrack* graphicalTrack = new GraphicalTrack(track->getUuid(),
                                                      x,y,
                                                      predictedX,predictedY);
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

QtRenderer::ColorManager::ColorManager()
  : availableColors({
                      Qt::red,
                      Qt::green,
                      Qt::blue,
                      Qt::yellow
                      })
{}


std::pair<QColor,QColor>
QtRenderer::ColorManager::setColorForTrack(GraphicalTrack* track)
{
  std::pair<QColor,QColor> colors = chooseColorForTrack(track);
  QPen pen(colors.first);
  QBrush brush(colors.second);

  track->setPen(pen);
  track->setBrush(brush);

  return colors;
}

std::pair<QColor,QColor>
QtRenderer::ColorManager::chooseColorForTrack(const GraphicalTrack* track)
{
  boost::uuids::uuid uuid = track->getUuid();
  QMap<boost::uuids::uuid,std::pair<QColor,QColor> >::const_iterator iter
      = trackColors_.find(uuid);
  if (iter == trackColors_.end())
  { // There is no color for given track
    std::pair<QColor,QColor> colors = generateNewColorForTrack(track);
    trackColors_[uuid] = colors;
    return colors;
  }
  else
  {
    return *iter;
  }
}

std::pair<QColor,QColor> QtRenderer::ColorManager
  ::generateNewColorForTrack(const GraphicalTrack* /*track*/) const
{
  QColor penColor(Qt::yellow);

  boost::random::uniform_int_distribution<> dist(0, availableColors.size()-1);
  std::size_t num = dist(randomGenerator_);
  QColor brushColor = availableColors.at(num);
  return std::make_pair(penColor,brushColor);
}

} // namespace Graphic

} // namespace View
