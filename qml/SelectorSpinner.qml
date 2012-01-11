import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
//import QtQuick 1.1

Item {
    property real spinnerRotation
    width: 100
    height: 100
    spinnerRotation: selectorBottom.rotation
    state: "classic"
    Image {
        id: selectorBottom
        source: "qrc:/images/selector-game-mode-spinner.png"
        anchors.fill: parent
        rotation: 30
    }
    Image {
        id: selectorTop
        source: "qrc:/images/selector-game-mode-top.png"
        anchors.fill: parent
    }
    states: [
        State {
            name: "classic"
            PropertyChanges {
                target: selectorBottom
                rotation: 2
            }
        },
        State {
            name: "party"
            PropertyChanges {
                target: selectorBottom
                rotation: 180
            }
        }
    ]
    transitions: [
        Transition {
            to: "*"
            NumberAnimation {
                target: selectorBottom
                duration: 1000
                properties: "rotation"
                easing.type: Easing.InOutBack
            }
        }
    ]
}
