#include "gameview.h"
#include "gamescene.h"
#include <QDebug>

#ifdef Q_WS_MAEMO_5
#include <mce/dbus-names.h>
#include <mce/mode-names.h>

static QDBusConnection dBusConnection = QDBusConnection::systemBus();
#endif

GameView::GameView() :
    QGraphicsView()
{
    qDebug() << "Setting scene";
    setScene(&gameScene);
    qDebug() << "Scene set";
    setRenderHint(QPainter::HighQualityAntialiasing,true);
    setRenderHint(QPainter::SmoothPixmapTransform,true);
#ifdef OS_IS_HARMATTAN
    setBackgroundBrush(QImage(":/images/background-harmattan.png"));
#else
    setBackgroundBrush(QImage(":/images/background.png"));
#endif
    setCacheMode(QGraphicsView::CacheBackground);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Nanoparticles"));
    setStyleSheet("QGraphicsView { border-style: none; }");
    setInteractive(true);
    setFrameShape(QFrame::NoFrame);
    qDebug() << "View setup done";

#ifdef Q_WS_MAEMO_5
    dBusInterface = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH,
                                       MCE_REQUEST_IF, dBusConnection, this);

    dBusConnection.connect(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF,
                           MCE_DISPLAY_SIG, this, SLOT(screenChange(const QDBusMessage &)));
#endif
}

void GameView::changeEvent(QEvent *event) {
#if defined(Q_WS_MAEMO_5) || defined(OS_IS_HARMATTAN)
    qDebug() << "Some event happened" << event->type();
    if(event->type()==QEvent::ActivationChange) {
        if(!isActiveWindow()) {
            gameScene.pauseGame();
        }
    }
#endif
}

void GameView::resizeEvent(QResizeEvent *event) {
    qDebug() << "resize";
    Q_UNUSED(event)
    scene()->setSceneRect(0,0,width(),height());
    GameScene* gameScene = (GameScene*)scene();
    gameScene->resized();
    qDebug() << "resized";
}

// MAEMO DBUS FOR LOCK EVENT

#ifdef Q_WS_MAEMO_5
void GameView::screenChange(const QDBusMessage &message)
{
    QString state = message.arguments().at(0).toString();
    if (!state.isEmpty()) {
        if (state == MCE_DISPLAY_OFF_STRING)
            gameScene.pauseGame();
    }
}
#endif
