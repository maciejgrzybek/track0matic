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
                               qreal widthFactor, qreal heightFactor,
                               qreal width, qreal height)
  : QGraphicsEllipseItem(x-width/2.0,y-height/2.0,width,height),
    prediction_(new QGraphicsEllipseItem(predictionX-widthFactor*width/2.0,
                                         predictionY-heightFactor*height/2.0,
                                         widthFactor*width,
                                         heightFactor*height)),
    uuid_(uuid)
{
  prediction_->setParentItem(this);
}

boost::uuids::uuid GraphicalTrack::getUuid() const
{
  return uuid_;
}

/******************************************************************************/

GraphicalStreet::GraphicalStreet(
    const std::shared_ptr<Model::WorldSnapshot::StreetSnapshot> snapshot)
  : street(snapshot)
{}

/******************************************************************************/

const double QtRenderer::varianceFactor_ = 500;

QtRenderer::QtRenderer(QtView* parent)
  : parent_(parent),
    mainWindow_(new QMainWindow()),
    scene_(new QGraphicsScene()),
    view_(new QGraphicsView(scene_))
{
  mainWindow_->setAttribute(Qt::WA_QuitOnClose,false);
  connect(this,SIGNAL(addTrackSignal(GraphicalTrack*)),
          SLOT(performAddTrack(GraphicalTrack*)));

  connect(this,SIGNAL(addStreetSignal(GraphicalStreet*)),
          SLOT(performAddStreet(GraphicalStreet*)));

  connect(this,SIGNAL(clearSceneSignal()),SLOT(performClearScene()));

  connect(this,SIGNAL(exitRequestedSignal()),SLOT(quitRequested()));

  connect(this,SIGNAL(showSignal()),mainWindow_,SLOT(show()));
  connect(this,SIGNAL(quitSignal()),mainWindow_,SLOT(close()));

  drawStaticGraphics();
  setupMenu();
  view_->scale(80000.0,80000.0);
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

void QtRenderer::addStreet(
    const std::shared_ptr<Model::WorldSnapshot::StreetSnapshot> street)
{
  GraphicalStreet* graphicalStreet = new GraphicalStreet(street);
  emit addStreetSignal(graphicalStreet); // invokeLater
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

void QtRenderer::performAddStreet(GraphicalStreet* graphicalStreet)
{
  const std::shared_ptr<Model::WorldSnapshot::StreetSnapshot> street
      = graphicalStreet->street;

  scene_->addLine(street->first->lon,street->first->lat,
                  street->second->lon,street->second->lat);

  delete graphicalStreet;
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
  qreal varX = track->getLongitudePredictionVariance();
  qreal varY = track->getLatitudePredictionVariance();

  GraphicalTrack* graphicalTrack = new GraphicalTrack(track->getUuid(),
                                                      x,y,
                                                      predictedX,predictedY,
                                                      varianceFactor_*varX,
                                                      varianceFactor_*varY);
  return graphicalTrack;
}

void QtRenderer::drawStaticGraphics()
{
  drawBackground();
}

void QtRenderer::drawBackground()
{
  parent_->requestMapData();
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
                      Qt::yellow,
                      Qt::black,
                      Qt::magenta,
                      Qt::cyan,
                      Qt::gray
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
