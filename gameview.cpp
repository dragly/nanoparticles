#include "gameview.h"
#include "gamescene.h"
#include <QDebug>

GameView::GameView() :
    QGraphicsView()
{
    qDebug() << "Setting scene";
    setScene(&gameScene);
    qDebug() << "Scene set";
    setRenderHint(QPainter::HighQualityAntialiasing,true);
    setRenderHint(QPainter::SmoothPixmapTransform,true);
    setBackgroundBrush(QImage(":/images/background.png"));
    setCacheMode(QGraphicsView::CacheBackground);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Reaktor"));
    qDebug() << "View setup done";
}

void GameView::resizeEvent(QResizeEvent *event) {
    qDebug() << "resize";
    Q_UNUSED(event)
    scene()->setSceneRect(0,0,width(),height());
    GameScene* gameScene = (GameScene*)scene();
    gameScene->resized();
    qDebug() << "resized";
}
