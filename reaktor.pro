#-------------------------------------------------
#
# Project created by QtCreator 2011-02-03T22:09:54
#
#-------------------------------------------------

QT       += core gui declarative
!android {
    QT       += opengl
}
TARGET = nanoparticles

maemo5 {
    message(Detected OS: Maemo5)
    CONFIG += qdbus
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
    TARGET = Nanoparticles
} else:win32 {
    message(Detected OS: Windows)
} else:unix {
    message(Detected OS: Desktop Linux)
    DEFINES += OS_IS_DESKTOP_LINUX
} else {
    message(Detected OS: Unknown)
}

message(Target: $$TARGET)

VERSION = 1.0.1
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

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian {
    ICON = nanoparticles.svg

    TARGET.UID3 = 0x200420AD
    #TARGET.UID3 = 0xE4365487
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
    android/AndroidManifest.xml

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

#maemo5 {
#    desktopfile.files = $${TARGET}.desktop
#    desktopfile.path = /usr/share/applications/hildon
#    INSTALLS += desktopfile
#}

#maemo5 {
#    icon.files = nanoparticles.png
#    icon.path = /usr/share/icons/hicolor/64x64/apps
#    INSTALLS += icon
#}

message(Current defines:)
message($$DEFINES)

