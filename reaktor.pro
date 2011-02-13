#-------------------------------------------------
#
# Project created by QtCreator 2011-02-03T22:09:54
#
#-------------------------------------------------

QT       += core gui
unix {
    !symbian {
        QT += opengl
    }
}
TARGET = Reaktor
VERSION = 0.3.0
TEMPLATE = app

SOURCES += main.cpp \
    particle.cpp \
    gamescene.cpp \
    gameview.cpp \
    button.cpp \
    gameobject.cpp

HEADERS  += \
    particle.h \
    gamescene.h \
    gameview.h \
    button.h \
    gameobject.h

FORMS    +=

CONFIG += mobility
MOBILITY = 

symbian {
    ICON = images/particle-positive.svg

    TARGET.UID3 = 0xe273c3f6
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
    # cone.lib, eikcore.lib and avkon.lib Symbian libraries
    LIBS += -lcone -leikcore -lavkon
}

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    LICENSE-font-NovaSquare.txt
