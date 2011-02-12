#include "gameview.h"
#include "gamescene.h"

GameView::GameView() :
    QGraphicsView()
{
}

void GameView::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    scene()->setSceneRect(0,0,width(),height());
    GameScene* gameScene = (GameScene*)scene();
    gameScene->resized();
}
