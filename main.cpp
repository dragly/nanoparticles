#include <QtGui>
#include <QtOpenGL/qgl.h>
#include "gamescene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameScene scene;

    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing,true);
    view.setRenderHint(QPainter::SmoothPixmapTransform,true);
    view.setBackgroundBrush(QPixmap(":/images/cheese.jpg"));
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Reaktor"));
    view.setViewport(new QGLWidget()); // IMPORTANT: Disabling this makes animations with images sluggish. Disable only if enteriely necessary, and try to find another option to draw smooth animations first.

    //view.resize(400, 300);
#if defined(Q_WS_S60)
    view.showMaximized();
#else
    view.show();
#endif

    return a.exec();
}
