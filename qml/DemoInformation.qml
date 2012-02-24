// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: demoInfoRoot
    width: 100
    height: 100
    color: "black"
    opacity: 0.9
    Behavior on opacity {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutCubic
        }
    }

    ImageButton {
        anchors {
            right: parent.right
            top: parent.top
            rightMargin: parent.width * 0.02
            topMargin: parent.width * 0.02
        }
        width: parent.width * 0.1
        height: parent.width * 0.1
        onClicked: {
//            demoInfoRoot.opacity = 0
            gameScene.setLevel(mainMenu.maxDemoLevel)
        }
        source: "qrc:/images/button-exit.png"
    }
    Column {
        anchors.fill: parent
        anchors.margins: parent.height * 0.2
        Text {
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: demoInfoRoot.height / 15
            color: "white"
            text: "We're sorry, but this is the end of the demo version..."
            wrapMode: Text.WordWrap
        }
        Text {
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: demoInfoRoot.height / 18
            color: "white"
            text: "You can buy the full version of Nanoparticles in " + mainMenu.buyGameStoreName + ". The full version includes an unlimited amount of levels, new particles and many hours of fun!"
            wrapMode: Text.WordWrap
        }
        ImageButton {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.4
            height: width * 68 / 180 // keep aspect ratio
            onClicked: {
                Qt.openUrlExternally(mainMenu.buyGameUrl)
            }
            source: mainMenu.buyGameImage
        }
    }
}
