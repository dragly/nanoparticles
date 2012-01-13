import QtQuick 1.0

Item {
    signal clicked
    property bool selected
    property string source
    property string selectedSource

    id: imgButton

    state: "released"
    selected: false

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed: imgButton.state = "pressed"
        onReleased: imgButton.state = "released"
        onClicked: parent.clicked()
    }

    Image {
        id: normalImage
        source: parent.source
        anchors.fill: parent
        opacity: 1
        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }
    }

    Image {
        id: selectedImage
        source: parent.selectedSource
        anchors.fill: parent
        opacity: 0
        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }
    }

    function refreshImage() {
        if(selected) {
            normalImage.opacity = 0
            selectedImage.opacity = 1
        } else {
            normalImage.opacity = 1
            selectedImage.opacity = 0
        }
    }

    onSelectedChanged: {
        refreshImage()
    }
    onSourceChanged: {
        refreshImage()
    }
    onSelectedSourceChanged: {
        refreshImage()
    }

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
                duration: 100
            }
        },

        Transition {
            from: "released"
            to: "pressed"
            PropertyAnimation {
                target: imgButton
                properties: "scale"
                duration: 100
            }
        }
    ]

}
