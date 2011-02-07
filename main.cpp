// all
#include <QtGui>
#include <QDebug>
// Symbian
#ifdef Q_WS_S60
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif
// not Symbian
#if defined(Q_WS_MAEMO_5)
#include <QtOpenGL/qgl.h>
#endif
// internal (all)
#include "gamescene.h"
#include "gameview.h"

int main(int argc, char *argv[])
{
#ifdef Q_WS_S60
    QApplication::setGraphicsSystem("openvg");
#endif
    QApplication a(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    GameScene scene;

    GameView view(0);
    view.setScene(&scene);
    view.setRenderHint(QPainter::Antialiasing,true);
    view.setRenderHint(QPainter::SmoothPixmapTransform,true);
    view.setBackgroundBrush(QImage(":/images/background.png"));
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Reaktor"));
#ifdef defined(Q_WS_MAEMO_5)
    view.setViewport(new QGLWidget()); // IMPORTANT: Disabling this makes animations with images sluggish. Disable only if enteriely necessary, and try to find another option to draw smooth animations first.
#endif


    //view.resize(400, 300);
#if defined(Q_WS_S60)
    view.showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    view.showFullScreen();
#else
    view.show();
#endif

    return a.exec();
}
