#include "gameview.h"

GameView::GameView(int bah) :
    QGraphicsView()
{
}

void GameView::resizeEvent(QResizeEvent *event) {
    scene()->setSceneRect(0,0,width(),height());
}
