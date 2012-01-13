// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.0

Item {
    property string text
    property bool clickable
    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.8
    }

    Text {
        anchors.centerIn: parent
        font.family: "NovaSquare"
        font.pixelSize: parent.height * 0.07
        text: parent.text
        color: "white"
    }
}
