// all
#include <QtGui>
#include <QDebug>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
// OpenGL
#ifndef NO_OPENGL
#include <QGLWidget>
#endif
// Symbian
#ifdef Q_WS_S60
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif
// internal (all)
#include "gamescene.h"
#include "gameview.h"

int main(int argc, char *argv[])
{
    // Register all QML mapped C++ classes
    qmlRegisterType<GameScene>("Nanoparticles", 1, 0, "GameScene");

    QApplication a(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    qDebug() << "Enabling settings";
    QCoreApplication::setOrganizationName("Dragly");
    QCoreApplication::setOrganizationDomain("dragly.org");
    QCoreApplication::setApplicationName("Nanoparticles");

    // Symbian specific code
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAPD(error,
          if (appUi) {
        // Lock application orientation into landscape
        appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
    }
    );
#endif
    qDebug() << "landscape loaded";

    qDebug() << "Starting view";
    GameView view;

#if QT_VERSION > 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    view.setAttribute(Qt::WA_LockLandscapeOrientation, true);
#endif // QT_VERSION < 0x040702
    qDebug() << "landscape loaded second run";

    // Font loading
#ifndef Q_OS_ANDROID
    qDebug() << "Starting database";
    QFontDatabase database;
    if(!database.addApplicationFont(":/fonts/novasquare/NovaSquare.ttf")) {
        qWarning() << "Could not load Nova font!";
    }
    qDebug() << "font loaded";
#endif

//#if defined(Q_WS_MAEMO_5) || defined(Q_OS_LINUX)
#ifndef NO_OPENGL
    qDebug() << "Using OpenGL";
    QGLWidget *glwidget = new QGLWidget();
    glwidget->showFullScreen();
    // IMPORTANT: Disabling this makes animations with images sluggish.
    // Disable only if enteriely necessary, and try to find another option to draw smooth animations first
    view.setViewport(glwidget);
#else
    qDebug() << "Not using OpenGL";
#endif
    //#endif
    qDebug() << "setViewport";

#if defined(Q_WS_S60)
    qDebug() << "Is Symbian!";
    view.showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    qDebug() << "Is Maemo!";
    view.showFullScreen();
#elif defined(OS_IS_HARMATTAN)
    qDebug() << "Is MeeGo!";
    view.showFullScreen();
#elif defined(Q_OS_ANDROID)
    qDebug() << "Is Android!";
    view.showFullScreen();
#elif defined(OS_IS_DESKTOP_LINUX) || defined(Q_OS_MAC) || defined(Q_OS_WIN)

#if defined(OS_IS_DESKTOP_LINUX)
    qDebug() << "Is Destkop Linux";
#elif defined(Q_OS_MAC)
    qDebug() << "Is Mac!";
#elif defined(Q_OS_WIN)
    qDebug() << "Is Windows!";
#endif
    QSettings settings;
    int viewMode = settings.value("viewMode", 0).toInt();
    if(viewMode == GameScene::ViewNormal) {
        view.showNormal();
    } else {
        view.showFullScreen();
    }
#else
    qDebug() << "Is some unknown OS!";
    if(viewMode == 0) {
        view.showNormal();
    } else {
        view.showFullScreen();
    }
#endif

    return a.exec();
}
