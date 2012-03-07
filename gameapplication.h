#ifndef GAMEAPPLICATION_H
#define GAMEAPPLICATION_H
#include <QDebug>

#ifdef Q_OS_SYMBIAN
#include <QSymbianEvent>
#include <w32std.h>
#endif
#include <QApplication>
#include "gameview.h"

class GameApplication : public QApplication
{
public:
    GameApplication( int argc, char** argv) : QApplication( argc, argv ), m_gameView(0) {}
    void setGameView(GameView* gameView ) {
        m_gameView = gameView;
    }

#ifdef Q_OS_SYMBIAN
protected:
    bool symbianEventFilter( const QSymbianEvent* symbianEvent );
#endif // Q_OS_SYMBIAN
private:
    GameView *m_gameView;
};

#endif // GAMEAPPLICATION_H
