import QtQuick 1.0
import Nanoparticles 1.0
import Qt.labs.particles 1.0


Item {

    property int level
    property int gameMode
    property int gameState
    property int remainingNegativeCharges
    property int remainingPositiveCharges
    // The contextGameScene object is passed to this QML object through the GameScene initialization in C++
    // We then set it to our internal property gameScene
    property GameScene gameScene
    gameScene: contextGameScene

    id: root
    width: 800
    height: 600
    state: "initial"
    level: gameScene.level
    gameMode: gameScene.gameMode
    gameState: gameScene.gameState
    remainingPositiveCharges: gameScene.remainingPositiveCharges
    remainingNegativeCharges: gameScene.remainingNegativeCharges

    onGameStateChanged: {
        if(gameState == GameScene.GameStarted) {
            statusText.text = "Nanoparticles"
            state = "paused"
            console.log("Game starteda")
        } else if(gameState == GameScene.GamePaused) {
            statusText.text = "Paused"
            state = "paused"
            console.log("Game pauseda")
        } else if(gameState == GameScene.GameOver) {
            console.log("Game Over")
            statusText.text = "Level failed"
            state = "paused"
        } else if(gameState == GameScene.GameRunning) {
            state = "running"
            console.log("Game runninga")
        }
    }

    onRemainingNegativeChargesChanged: {
        console.log(remainingNegativeCharges)
        if(remainingNegativeCharges < 1) {
            negativeCharges.source = "qrc:/images/button-empty-charge.png"
            negativeCharges.selectedSource = "qrc:/images/button-empty-charge-selected.png"
        } else {
            negativeCharges.source = "qrc:/images/button-negative-charge.png"
            negativeCharges.selectedSource = "qrc:/images/button-negative-charge-selected.png"
        }
    }

    states: [
        State {
            name: "running"
            PropertyChanges { target: menuBack; opacity: 0.0 }
            AnchorChanges { target: levelDown; anchors.top: parent.bottom }
            AnchorChanges { target: levelUp; anchors.top: parent.bottom }
            AnchorChanges { target: exit; anchors.right: undefined; anchors.left: parent.right }
            AnchorChanges { target: about; anchors.right: undefined;  anchors.left: parent.right }
            AnchorChanges { target: spinner; anchors.left: undefined;  anchors.right: parent.left }
        },
        State {
            name: "paused"
            PropertyChanges { target: continu; scale: 1 }
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
                ParallelAnimation {
                    AnchorAnimation {
                        targets: [ levelDown, levelUp, about, exit, spinner ]
                        duration: 500
                        easing.type: Easing.InBack
                    }
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
        }
    ]

    onGameModeChanged: {
        if(gameMode == GameScene.ModeClassic) {
            spinner.state = "classic"
        } else {
            spinner.state = "party"
        }
    }

    Item {
        id: menuBack
        anchors.fill: parent

        Rectangle{
            color: "#000000"
            opacity: 0.6
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

        SelectorSpinner {
            id: spinner

            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.02
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.width * 0.02
            width: parent.width * 0.25
            height: parent.width * 0.25
            MouseArea {
                onClicked: {
                    if(gameScene.gameMode == GameScene.ModeClassic) {
                        gameScene.gameMode = GameScene.ModeParty
                    } else {
                        gameScene.gameMode = GameScene.ModeClassic
                    }
                }
                anchors.fill: parent
            }
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
    Behavior on level {
        SequentialAnimation {
            id: bounceAround
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
                duration: 300
                easing.type: Easing.OutCubic
            }
        }
    }

    AboutDialog {
        id: aboutDialog
        opacity: 0
        z: 2
        anchors.fill: parent
    }

    Rectangle {
        opacity: 0.7
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

    ImageButton {
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
        source: "qrc:/images/button-positive-charge.png"
        selectedSource: "qrc:/images/button-positive-charge-selected.png"
    }

    ImageButton {
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
        source: "qrc:/images/button-negative-charge.png"
        selectedSource: "qrc:/images/button-negative-charge-selected.png"
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
