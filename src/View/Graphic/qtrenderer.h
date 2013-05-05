#ifndef VIEW_GRAPHIC_QTRENDERER_H
#define VIEW_GRAPHIC_QTRENDERER_H

#include <memory>

#include <boost/random/mersenne_twister.hpp>
#include <boost/uuid/uuid.hpp>

#include <QMap>
#include <QObject>
#include <QGraphicsEllipseItem>

#include <Model/modelsnapshot.h>

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
                 qreal predictionX, qreal predictionY,
                 qreal widthFactor = 2, qreal heightFactor = 2,
                 qreal width = 0.0001, qreal height = 0.0001);

  boost::uuids::uuid getUuid() const;

private:
  QGraphicsEllipseItem* prediction_;
  const boost::uuids::uuid uuid_;
};

// holds street snapshot, as long as Qt needs it.
// Wrapper to omit introducing std::shared_ptr to Qt meta types.
// TODO Maybe later it should be drawable item (like GraphicalTrack)?
struct GraphicalStreet
{
  GraphicalStreet(const std::shared_ptr<Model::WorldSnapshot::StreetSnapshot>);

  const std::shared_ptr<Model::WorldSnapshot::StreetSnapshot> street;
};

class QtRenderer : public QObject
{
  Q_OBJECT
public:
  QtRenderer(QtView* parent);
  virtual ~QtRenderer();

  void show();
  void addTrack(const Track*); // TODO there should be TrackSnapshot,
                               // instead of Track
  void addStreet(const std::shared_ptr<Model::WorldSnapshot::StreetSnapshot>);
  void clearScene();
  void requestExit();
  void close();

signals:
  void addTrackSignal(GraphicalTrack*);
  void addStreetSignal(GraphicalStreet*);
  void clearSceneSignal();
  void exitRequestedSignal();
  void showSignal();
  void quitSignal();

protected slots:
  void performAddTrack(GraphicalTrack*);
  void performAddStreet(GraphicalStreet*);
  void performClearScene();
  void quitRequested();

private:
  class ColorManager
  {
  public:
    ColorManager();

    std::pair<QColor,QColor> setColorForTrack(GraphicalTrack*);

    // TODO rewrite it to be done in cleaner way and to wiser choose colors
    //  (e.g. based on distance between tracks
    //    - to avoid situation when two Tracks which are close to each other,
    //      have the same color assigned)
    std::pair<QColor,QColor> chooseColorForTrack(const GraphicalTrack*);

  private:
    std::pair<QColor,QColor>
      generateNewColorForTrack(const GraphicalTrack*) const;

    QMap<boost::uuids::uuid,std::pair<QColor,QColor> > trackColors_;
    const QVector<QColor> availableColors;

    mutable boost::random::mt19937 randomGenerator_;
  };

  static GraphicalTrack* transformTrackFromSnapshot(const Track*);
  void drawStaticGraphics();
  void drawBackground();
  void setupMenu();

  ColorManager colorManager_;

  QtView* parent_;

  QMainWindow* mainWindow_;
  QGraphicsScene* scene_;
  QGraphicsView* view_;
};

} // namespace Graphic

} // namespace View

#endif // VIEW_GRAPHIC_QTRENDERER_H
