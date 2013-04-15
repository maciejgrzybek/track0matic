#include "qtrenderer.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>

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

QtRenderer::QtRenderer(std::size_t width, std::size_t height)
  : scene_(new QGraphicsScene(0.0,0.0,width,height)),
    view_(new QGraphicsView(scene_))
{}

QtRenderer::~QtRenderer()
{
  delete view_;
  delete scene_;
}

void QtRenderer::show()
{
  view_->show();
}

void QtRenderer::addTrack(const Track* track)
{
  GraphicalTrack* graphicalTrack = transformTrackFromSnapshot(track);
  scene_->addItem(graphicalTrack);
}

GraphicalTrack* QtRenderer::transformTrackFromSnapshot(const Track* track)
{
  qreal x = track->getLongitude();
  qreal y = track->getLatitude();

  GraphicalTrack* graphicalTrack = new GraphicalTrack(track->getUuid(),x,y);
  return graphicalTrack;
}

} // namespace Graphic

} // namespace View
