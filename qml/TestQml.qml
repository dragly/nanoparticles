//import QtQuick 1.0
// must be Qt 4.7 for Maemo
import Qt 4.7

Rectangle {
    width: 400
    height: 300
    Rectangle {
        width: 100
        height: 150
        anchors: none
        x: 100
        y: 100
        color: "red"
    }
    Rectangle {
        id: partner
        width: 100
        height: 100
        x: 10
        y: 10
        color: "blue"
    }
}
