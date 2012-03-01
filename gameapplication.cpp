#include "gameapplication.h"
#ifdef Q_OS_SYMBIAN
GameApplication::symbianEventFilter( const QSymbianEvent* symbianEvent ) {
    const TWsEvent *event = symbianEvent->windowServerEvent();

    if( !event ) {
        return false;
    }

    switch( event->Type() ) {
    case EEventFocusGained: {
        qDebug() << "symbianEventFilter(): Focus gained";
        break;
    }
    case EEventFocusLost: {
        qDebug() << "symbianEventFilter(): Focus lost, pausing game.";
        if(m_gameView) {
            m_gameView->pauseGame();
        }
        break;
    }
    default:
        break;
    }

    // Always return false so we don't stop
    // the event from being processed
    return false;
}
#endif // Q_OS_SYMBIAN
