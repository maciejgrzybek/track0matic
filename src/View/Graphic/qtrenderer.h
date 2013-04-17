#ifndef VIEW_GRAPHIC_QTRENDERER_H
#define VIEW_GRAPHIC_QTRENDERER_H

#include <memory>

#include <boost/random/mersenne_twister.hpp>
#include <boost/uuid/uuid.hpp>

#include <QMap>
#include <QObject>
#include <QGraphicsEllipseItem>

class QGraphicsScene;
class QGraphicsView;
class QMainWindow;
class Track;

namespace View
{

namespace Graphic
{

class QtView;

class GraphicalTrack : public QGraphicsEllipseItem
{
public:
  GraphicalTrack(boost::uuids::uuid uuid,
                 qreal x, qreal y,
                 qreal width = 5, qreal height = 5);

  boost::uuids::uuid getUuid() const;
  void moveBasedOnOther(const GraphicalTrack*);

private:
  const boost::uuids::uuid uuid_;
};

class QtRenderer : public QObject
{
  Q_OBJECT
public:
  QtRenderer(std::size_t width, std::size_t height, QtView* parent);
  virtual ~QtRenderer();

  void show();
  void addTrack(const Track*);
  void clearScene();
  void requestExit();
  void close();

signals:
  void addTrackSignal(GraphicalTrack*);
  void clearSceneSignal();
  void exitRequestedSignal();
  void showSignal();
  void quitSignal();

protected slots:
  void performAddTrack(GraphicalTrack*);
  void quitRequested();

private:
  static GraphicalTrack* transformTrackFromSnapshot(const Track*);
  void drawStaticGraphics();
  void drawBackground();
  void setupMenu();

  // TODO rewrite it to be done in cleaner way and to wiser choose colors
  //  (e.g. based on distance between tracks
  //    - to avoid situation when two Tracks which are close to each other,
  //      have the same color assigned)
  void chooseColorForTrack(GraphicalTrack*);

  boost::random::mt19937 randomGenerator_;

  QtView* parent_;

  QMainWindow* mainWindow_;
  QGraphicsScene* scene_;
  QGraphicsView* view_;

  QMap<boost::uuids::uuid,GraphicalTrack*> tracks_;
};

} // namespace Graphic

} // namespace View

#endif // VIEW_GRAPHIC_QTRENDERER_H
