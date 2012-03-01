#-------------------------------------------------
#
# Project created by QtCreator 2011-02-03T22:09:54
#
#-------------------------------------------------

#DEFINES += BENCHMARK

QT       += core gui declarative

# To test on non-OpenGL devices, switch the commenting on the next two lines. For testing on non-OpenGL
# Android devices, remember to remove the QtOpenGL dependency in the Projects settings.
QT       += opengl
#DEFINES += NO_OPENGL

# To build demo, set this to true and change package name to nanoparticles-demo for Maemo and Meego
# Change to org.dragly.nanoparticlesdemo for Android
ISDEMO = true

contains(ISDEMO, true) {
    message(Is demo)
    DEFINES += IS_DEMO
    TARGET = nanoparticles-demo
} else {
    message(Is not demo)
    TARGET = nanoparticles
}

maemo5 {
    message(Detected OS: Maemo5)
    CONFIG += qdbus
} else:harmattan { # harmattan (to become a keyword of its own)
    message(Harmattan)
    DEFINES += OS_IS_HARMATTAN
} else:android {
    message(Detected OS: Android)
    DEFINES += OS_IS_ANDROID
} else:contains(MEEGO_EDITION,harmattan) { # harmattan (to become a keyword of its own)
    message(Detected OS: Harmattan)
    DEFINES += OS_IS_HARMATTAN
} else:macx {
    message(Detected OS: Mac OS X)
} else:symbian {
    message(Detected OS: Symbian)
    contains(ISDEMO, true) {
        TARGET = NanoparticlesDemo
        DEPLOYMENT.display_name = Nanoparticles Demo
    } else {
        TARGET = Nanoparticles
    }
} else:win32 {
    message(Detected OS: Windows)
} else:unix {
    message(Detected OS: Desktop Linux)
    DEFINES += OS_IS_DESKTOP_LINUX
    desktopfile.files = $${TARGET}_ubuntu.desktop
    desktopfile.path = /usr/share/applications
    icon.files = $${TARGET}.png
    icon.path = /usr/share/icons/hicolor/scalable/apps
    INSTALLS += icon desktopfile
} else {
    message(Detected OS: Unknown)
}

message(Target: $$TARGET)

VERSION = 2.0.1
TEMPLATE = app

SOURCES += main.cpp \
    particle.cpp \
    gamescene.cpp \
    gameview.cpp \
    gameobject.cpp \
    gameapplication.cpp

HEADERS  += \
    particle.h \
    gamescene.h \
    gameview.h \
    gameobject.h \
    utils.h \
    gameapplication.h

folder_01.source = qml
folder_01.target =
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# Icon settings
symbian {
    ICON = nanoparticles.svg
    contains(ISDEMO, true) {
        TARGET.UID3 = 0x200420AE
    } else {
        TARGET.UID3 = 0x200420AD
    }
    #TARGET.UID3 = 0xE4365487
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
    #TARGET.CAPABILITY += ReadUserData WriteUserData UserEnvironment NetworkServices # added to avoid crash
    # cone.lib, eikcore.lib and avkon.lib Symbian libraries
    LIBS += -lcone -leikcore -lavkon
} else:macx {
    ICON = nanoparticles.icns
}
# Maemo 5 and Harmattan are defined in deployment.pri

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    nanoparticles_harmattan.desktop \
    nanoparticles.desktop \
    LICENSE-font-NovaSquare.txt \
    AboutDialog.qml \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_ubuntu/rules \
    qtc_packaging/debian_ubuntu/README \
    qtc_packaging/debian_ubuntu/copyright \
    qtc_packaging/debian_ubuntu/control \
    qtc_packaging/debian_ubuntu/compat \
    qtc_packaging/debian_ubuntu/changelog \
    android/AndroidManifest.xml \
    android/src/eu/licentia/necessitas/mobile/QtSensors.java \
    android/src/eu/licentia/necessitas/mobile/QtMediaPlayer.java \
    android/src/eu/licentia/necessitas/mobile/QtFeedback.java \
    android/src/eu/licentia/necessitas/mobile/QtAndroidContacts.java \
    android/src/eu/licentia/necessitas/mobile/QtSystemInfo.java \
    android/src/eu/licentia/necessitas/mobile/QtLocation.java \
    android/src/eu/licentia/necessitas/mobile/QtCamera.java \
    android/src/eu/licentia/necessitas/industrius/QtActivity.java \
    android/src/eu/licentia/necessitas/industrius/QtSurface.java \
    android/src/eu/licentia/necessitas/industrius/QtApplication.java \
    android/src/eu/licentia/necessitas/industrius/QtLayout.java \
    android/src/eu/licentia/necessitas/ministro/IMinistro.aidl \
    android/src/eu/licentia/necessitas/ministro/IMinistroCallback.aidl \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/drawable-hdpi/icon.png \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/res/values-nb/strings.xml \
    android/res/layout/splash.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-fa/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable/icon.png \
    android/res/values-de/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-fr/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-id/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-et/strings.xml \
    android/AndroidManifest.xml \
    android/version.xml

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

message(Current defines:)
message($$DEFINES)

