#ifndef VIEW_GRAPHIC_QTRENDERER_H
#define VIEW_GRAPHIC_QTRENDERER_H

#include <memory>

#include <boost/uuid/uuid.hpp>

#include <QGraphicsEllipseItem>

class QGraphicsScene;
class QGraphicsView;
class Track;

namespace View
{

namespace Graphic
{

class GraphicalTrack : public QGraphicsEllipseItem
{
public:
  GraphicalTrack(boost::uuids::uuid uuid,
                 qreal x, qreal y,
                 qreal width = 5, qreal height = 5);

private:
  const boost::uuids::uuid uuid_;
};

class QtRenderer
{
public:
  QtRenderer(std::size_t width, std::size_t height);
  ~QtRenderer();

  void show();
  void addTrack(const Track*);

private:
  static GraphicalTrack* transformTrackFromSnapshot(const Track*);

  QGraphicsScene* scene_;
  QGraphicsView* view_;
};

} // namespace Graphic

} // namespace View

#endif // VIEW_GRAPHIC_QTRENDERER_H
