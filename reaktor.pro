#-------------------------------------------------
#
# Project created by QtCreator 2011-02-03T22:09:54
#
#-------------------------------------------------

QT       += core gui
QT += opengl

TARGET = reaktor
TEMPLATE = app


SOURCES += main.cpp \
    particle.cpp \
    gamescene.cpp

HEADERS  += \
    particle.h \
    gamescene.h

FORMS    +=

CONFIG += mobility
MOBILITY = 

symbian {
    TARGET.UID3 = 0xe273c3f6
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}

RESOURCES += \
    resources.qrc
