#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include "gamescene.h"

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView();

signals:

public slots:

protected:
    void resizeEvent(QResizeEvent *event);

private:
    GameScene gameScene;

};

#endif // GAMEVIEW_H
