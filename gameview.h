#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView();

signals:

public slots:

protected:
    void resizeEvent(QResizeEvent *event);

};

#endif // GAMEVIEW_H
