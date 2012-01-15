//import QtQuick 1.0
// must be Qt 4.7 for Maemo
import Qt 4.7

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
