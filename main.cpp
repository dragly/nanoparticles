#include <QtGui>
#include <QtOpenGL/qgl.h>
#include <QDebug>
#include "gamescene.h"
#include "gameview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    GameScene scene;

    GameView view(0);
    view.setScene(&scene);
    view.setRenderHint(QPainter::Antialiasing,true);
    view.setRenderHint(QPainter::SmoothPixmapTransform,true);
    view.setBackgroundBrush(QImage(":/images/background.png"));
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Reaktor"));
    view.setViewport(new QGLWidget()); // IMPORTANT: Disabling this makes animations with images sluggish. Disable only if enteriely necessary, and try to find another option to draw smooth animations first.

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
