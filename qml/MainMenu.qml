import QtQuick 1.0

// The gameScene object is passed to this QML object through the GameScene initialization in C++

Item {

    property int level

    id: root
    width: 400
    height: 300
    state: "initial"
    level: 1

    states: [
        State {
            name: "initial"
            PropertyChanges { target: statusText; text: "Nanoparticles" }
            PropertyChanges { target: retry; visible: false }
            AnchorChanges { target: pause; anchors.right: undefined;  anchors.left: parent.right }
            AnchorChanges { target: positiveCharges; anchors.right: undefined;  anchors.left: parent.right }
            AnchorChanges { target: negativeCharges; anchors.right: undefined;  anchors.left: parent.right }
        },
        State {
            name: "running"
            PropertyChanges { target: menuBack; opacity: 0.0 }
            AnchorChanges { target: levelDown; anchors.top: parent.bottom }
            AnchorChanges { target: levelUp; anchors.top: parent.bottom }
            AnchorChanges { target: exit; anchors.right: undefined; anchors.left: parent.right }
            AnchorChanges { target: about; anchors.right: undefined;  anchors.left: parent.right }
        },
        State {
            name: "paused"
            PropertyChanges { target: statusText; text: "Paused" }
            PropertyChanges { target: continu; scale: 1 }
            AnchorChanges { target: pause; anchors.right: undefined;  anchors.left: parent.right }
            AnchorChanges { target: positiveCharges; anchors.right: undefined;  anchors.left: parent.right }
            AnchorChanges { target: negativeCharges; anchors.right: undefined;  anchors.left: parent.right }
        },
        State {
            name: "gameOver"
            PropertyChanges { target: statusText; text: "Level failed" }
            AnchorChanges { target: pause; anchors.right: undefined;  anchors.left: parent.right }
            AnchorChanges { target: positiveCharges; anchors.right: undefined;  anchors.left: parent.right }
            AnchorChanges { target: negativeCharges; anchors.right: undefined;  anchors.left: parent.right }
        }
    ]

    transitions: [
        Transition {
            to: "running"
            PropertyAnimation {
                properties: "opacity"
                duration: 500
                easing.type: Easing.InQuad
            }
            SequentialAnimation {
                AnchorAnimation {
                    targets: [ levelDown, levelUp, about, exit ]
                    duration: 500
                    easing.type: Easing.InBack
                }
                AnchorAnimation {
                    targets: [ pause, positiveCharges, negativeCharges, timeLeft ]
                    duration: 500
                    easing.type: Easing.OutBack
                }
            }
        },
        Transition {
            to: "paused"
            NumberAnimation {
                properties: "opacity"
                duration: 500
                easing.type: Easing.OutQuad
            }
            AnchorAnimation {
                duration: 500
                easing.type: Easing.OutBack
            }
        },
        Transition {
            to: "gameOver"
            NumberAnimation {
                properties: "opacity"
                duration: 500
                easing.type: Easing.OutQuad
            }
            AnchorAnimation {
                duration: 500
                easing.type: Easing.OutBack
            }
        }
    ]

    Item {
        id: menuBack
        anchors.fill: parent

        Rectangle{
            color: "#000000"
            opacity: 0.8
            anchors.fill: parent
        }

        Text {
            id: levelText
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.1
            color: "white"
            text: "Level " + level
            font.family: "NovaSquare"
            font.pixelSize: parent.height * 0.07
            onTextChanged: bounceAround.start()
        }

        Text {
            id: statusText
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.18
            color: "white"
            text: ""
            font.family: "NovaSquare"
            font.pixelSize: root.height * 0.1
        }

        Timer {
            id: continueTimer
            interval: 500; running: false; repeat: false
            onTriggered: gameScene.continueGame()
        }

        ImageButton {
            id: continu
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.3
            source: "qrc:/images/button-continue.png"
            width: parent.width * 0.22
            height: parent.width * 0.22
            onClicked: {
                root.state = "running"
                continueTimer.start()
            }
        }

        Timer {
            id: retryTimer
            interval: 500; running: false; repeat: false
            onTriggered: gameScene.retryGame()
        }

        ImageButton {
            id: retry
            x: 280
            y: 110
            source: "qrc:/images/button-retry.png"
            width: parent.width * 0.1
            height: parent.width * 0.1
            anchors.verticalCenter: continu.verticalCenter
            anchors.left: continu.right
            anchors.leftMargin: parent.width * 0.01
            onClicked: {
                root.state = "running"
                retryTimer.start()
            }
        }

        ImageButton {
            id: levelUp
            anchors.left: continu.right
            anchors.top: continu.bottom
            anchors.leftMargin: -parent.width * 0.05
            width: parent.width * 0.1
            height: parent.width * 0.1
            onClicked: {
                gameScene.clickedNextLevelButton()
            }
            source: "qrc:/images/button-levelup.png"
            anchors.horizontalCenterOffset: 53
            anchors.verticalCenterOffset: 56
            opacity: gameScene.level < gameScene.highestLevel
            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }

        ImageButton {
            id: levelDown
            anchors.right: continu.left
            anchors.top: continu.bottom
            anchors.rightMargin: -parent.width * 0.05
            width: parent.width * 0.1
            height: parent.width * 0.1

            onClicked: {
                gameScene.clickedPrevLevelButton()
            }
            source: "qrc:/images/button-leveldown.png"
            opacity: gameScene.level > 1
            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }

        ImageButton {
            id: exit
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: parent.width * 0.02
            anchors.topMargin: parent.width * 0.02
            width: parent.width * 0.1
            height: parent.width * 0.1
            onClicked: {
                gameScene.exitGame()
            }
            source: "qrc:/images/button-exit.png"
            anchors.horizontalCenterOffset: 53
            anchors.verticalCenterOffset: 56
        }

        ImageButton {
            id: about
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: parent.width * 0.02
            anchors.topMargin: parent.width * 0.22
            width: parent.width * 0.1
            height: parent.width * 0.1
            onClicked: {
                aboutDialog.opacity = 1.0
            }
            source: "qrc:/images/button-info.png"

        }
    }

    AboutDialog {
        id: aboutDialog
        opacity: 0
        z: 2
        anchors.fill: parent
    }

    SequentialAnimation {
        id: bounceAround
        running: false
        NumberAnimation {
            target: levelText
            property: "anchors.topMargin"
            to: root.height * 0.06
            duration: 100
            easing.type: Easing.InCubic
        }
        NumberAnimation {
            target: levelText
            property: "anchors.topMargin"
            to: root.height * 0.1
            duration: 100
            easing.type: Easing.OutCubic
        }
    }

    Rectangle {
        opacity: 0.8
        color: "black"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: root.width * 0.16
        z: -1000
    }

    ImageButton {
        id: pause
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: parent.width * 0.02
        anchors.topMargin: parent.width * 0.02
        width: parent.width * 0.1
        height: parent.width * 0.1
        onClicked: {
            gameScene.pauseGame()
        }
        source: "qrc:/images/button-pause.png"

    }

    ChargeButton {
        id: positiveCharges
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: parent.width * 0.02
        anchors.topMargin: parent.height * 0.25
        width: parent.width * 0.1
        height: parent.width * 0.1
        selected: true
        onClicked: {
            gameScene.clickedPositiveButton()
            selected = true
            negativeCharges.selected = false
        }
        source: "qrc:/images/particle-positive.png"
    }

    ChargeButton {
        id: negativeCharges
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: parent.width * 0.02
        anchors.topMargin: parent.height * 0.45
        width: parent.width * 0.1
        height: parent.width * 0.1
        selected: false
        onClicked: {
            gameScene.clickedNegativeButton()
            selected = true
            positiveCharges.selected = false
        }
        source: "qrc:/images/particle-negative.png"
    }

    Text {
        id: timeLeft
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: parent.width * 0.03
        anchors.bottomMargin: parent.width * 0.04
        text: gameScene.levelTime
        color: "white"
        font.pixelSize: parent.width * 0.07
        font.family: "NovaSquare"
    }

}
