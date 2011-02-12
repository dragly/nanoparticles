#include "gameview.h"
#include "gamescene.h"

GameView::GameView() :
    QGraphicsView()
{
    setScene(&gameScene);
    setRenderHint(QPainter::Antialiasing,true);
    setRenderHint(QPainter::SmoothPixmapTransform,true);
    setBackgroundBrush(QImage(":/images/background.png"));
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Reaktor"));
}

void GameView::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    scene()->setSceneRect(0,0,width(),height());
    GameScene* gameScene = (GameScene*)scene();
    gameScene->resized();
}
