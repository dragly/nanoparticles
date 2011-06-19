#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#ifdef Q_WS_MAEMO_5
#include <QtDBus>
#endif
#include "gamescene.h"

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView();

signals:

public slots:
#ifdef Q_WS_MAEMO_5
    void screenChange(const QDBusMessage &message);
#endif

protected:
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *event);

private:
    GameScene gameScene;
#ifdef Q_WS_MAEMO_5
    QDBusInterface* dBusInterface;
#endif
};

#endif // GAMEVIEW_H
