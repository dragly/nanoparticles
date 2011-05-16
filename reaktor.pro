#-------------------------------------------------
#
# Project created by QtCreator 2011-02-03T22:09:54
#
#-------------------------------------------------

QT       += core gui declarative
#unix {
#    !symbian {
        QT += opengl
#    }
#}
TARGET = Nanoparticles
VERSION = 0.5.0
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

#CONFIG += mobility
#MOBILITY =

folder_01.source = qml
folder_01.target =
DEPLOYMENTFOLDERS = folder_01

symbian {
    ICON = untitled.svg

    TARGET.UID3 = 0xe273c3f6
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
    #TARGET.CAPABILITY += ReadUserData WriteUserData UserEnvironment NetworkServices # added to avoid crash
    # cone.lib, eikcore.lib and avkon.lib Symbian libraries
    LIBS += -lcone -leikcore -lavkon
}

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    LICENSE-font-NovaSquare.txt \
    AboutDialog.qml

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()
