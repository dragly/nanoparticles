import QtQuick 1.0

Image {
    id: imgButton

    state: "released"
    signal clicked

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed: imgButton.state = "pressed"
        onReleased: imgButton.state = "released"
        onClicked: parent.clicked()
    }
    source: "qrc:/images/button-info.png"
    states: [
        State {
            name: "pressed"
            PropertyChanges{
                target: imgButton
                scale: 1.2
            }
        },
        State {
            name: "released"
            PropertyChanges{
                target: imgButton
                scale: 1.0
            }
        }
    ]

    transitions: [
        Transition {
            from: "pressed"
            to: "released"
            PropertyAnimation {
                target: imgButton
                properties: "scale"
                duration: 50
            }
        },

        Transition {
            from: "released"
            to: "pressed"
            PropertyAnimation {
                target: imgButton
                properties: "scale"
                duration: 50
            }
        }
    ]

}
