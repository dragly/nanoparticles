//import QtQuick 1.0
// must be Qt 4.7 for Maemo
import Qt 4.7
import Nanoparticles 1.0


Item {

    property int level
    property int levelTime
    property int gameMode
    property int gameState
    property int remainingNegativeCharges
    property int remainingPositiveCharges
    property int remainingSpecialCharges
    property int selectedType
    property int viewMode
    property bool instructionShown
    property bool dashboardButtonVisible
    // The contextGameScene object is passed to this QML object through the GameScene initialization in C++
    // We then set it to our internal property gameScene
    property GameScene gameScene
    gameScene: contextGameScene

    id: root
    dashboardButtonVisible: true
    width: 800
    height: 600
    state: "initial"
    viewMode: gameScene.viewMode
    level: gameScene.level
    levelTime: gameScene.levelTime
    gameMode: gameScene.gameMode
    gameState: gameScene.gameState
    selectedType: gameScene.selectedType
    remainingPositiveCharges: gameScene.remainingPositiveCharges
    remainingNegativeCharges: gameScene.remainingNegativeCharges
    remainingSpecialCharges: gameScene.remainingSpecialCharges

    function prepareInstructions() {
        gameScene.gameState = GameScene.GameInstructionPause
        instructionText.opacity = 1
        instructionText.clickable = false
        instructionClickableTimer.start()
    }

    onLevelTimeChanged: {
        if(gameState == GameScene.GameRunning) {
            console.log("Level time changed")
            // Check if we should issue instructions
            if(gameMode == GameScene.ModeClassic) {
                if(level == 1) {
                    var t1 = 25;
                    var t2 = 17;
                    var t3 = 10;
                    if(levelTime == t1 || levelTime == t2 || levelTime == t3) {
                        prepareInstructions()
                        if(levelTime == t1) {
                            instructionText.text = "<center><p>Welcome!</p><p>You are the green charge.<br>Avoid hitting the purple charges,<br>they are dangerous.<br>You can move the green charge by placing<br>other charges anywhere on the screen.</p></center>"
                        } else if(levelTime == t2) {
                            instructionText.text = "<center>You can also select blue charges<br>by clicking on the blue button to the right.<br>The red charges push away the green one,<br>while the blue charges attract it.</center>"
                        } else if(levelTime == t3) {
                            instructionText.text = "<center>The time is shown in the lower right corner.<br>When the time runs out,<br/>you will move on to the next quantum state!<br/><br/>You might call them levels if you like.</center>"
                        }
                    }
                }
            } else if(gameMode == GameScene.ModeParty) {
                if(!instructionShown) {
                    var l1 = 1;
                    var l2 = 7;
                    var l3 = 14;
                    if(level == l1 || level == l2 || level == l3) {
                        prepareInstructions()
                        switch(level) {
                        case l1:
                            instructionText.text = "<center><center><p>Welcome to party mode!</p><p>New particles await you.</p><p>Collide with the light blue particles<br/>to repel all enemies.</p></center>"
                            break;
                        case l2:
                            instructionText.text = "<center><p>The clocks are ticking.</p><p>Let's make everybody slow down.</p></center>"
                            break;
                        case l3:
                            instructionText.text = "<center><p>Teleport away!</p><p>Wouldn't it be nice if we could<br/>go to a different place?</p><p>Hint: Use the button to the right.</p></center>"
                            break;
                        }
                    }
                }
                instructionShown = true
            }
        }
    }

    onGameModeChanged: {
        if(gameMode == GameScene.ModeParty) {
            spinner.state = "party"
        } else {
            spinner.state = "classic"
        }
    }

    onLevelChanged: {
        instructionShown = false
    }

    onSelectedTypeChanged: {
        positiveCharges.selected = false
        negativeCharges.selected = false
        specialCharges.selected = false
        if(selectedType == GameScene.ParticlePositive) {
            positiveCharges.selected = true
        } else if(selectedType == GameScene.ParticleNegative) {
            negativeCharges.selected = true
        } else if(selectedType == GameScene.ParticleSpecial) {
            specialCharges.selected = true
        }
    }

    onGameStateChanged: {
        console.log("Game state changed")
        if(gameState == GameScene.GameStarted) {
            statusText.text = "Nanoparticles"
            state = "paused"
            console.log("Game starteda")
        } else if(gameState == GameScene.GamePaused) {
            if(gameScene.levelUpgrade) {
                statusText.text = "Level up!"
            } else {
                statusText.text = "Paused"
            }
            state = "paused"
            console.log("Game pauseda")
        } else if(gameState == GameScene.GameOver) {
            console.log("Game Over")
            statusText.text = "Level failed"
            state = "paused"
        } else if(gameState == GameScene.GameRunning) {
            console.log("Game running")
            state = "running"
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
            AnchorChanges { target: specialCharges; anchors.right: undefined;  anchors.left: parent.right }
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
                    targets: [ pause, positiveCharges, negativeCharges, specialCharges, timeLeft ]
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

    InstructionText {
        id: instructionText
        anchors.fill: parent
        opacity: 0
        z: 10000
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if(instructionText.clickable) {
                    gameScene.gameState = GameScene.GameRunning
                    instructionText.opacity = 0
                }
            }
        }
    }
    Timer {
        id: instructionClickableTimer
        interval: 500
        repeat: false
        onTriggered: {
            instructionText.clickable = true
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
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: parent.height * 0.1
            }
            color: "white"
            text: "Level " + level
            font.family: "NovaSquare"
            font.pixelSize: parent.height * 0.07
        }

        Text {
            id: statusText
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: parent.height * 0.18
            }
            color: "white"
            text: ""
            font.family: "NovaSquare"
            font.pixelSize: root.height * 0.1
        }

        SelectorSpinner {
            id: spinner

            anchors {
                left: parent.left
                leftMargin: parent.width * 0.02
                bottom: parent.bottom
                bottomMargin: parent.width * 0.02
            }
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
            onTriggered: {
                console.log("MainMenu.qml: Set game state to running")
                gameScene.gameState = GameScene.GameRunning
            }
        }

        ImageButton {
            id: continu
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: parent.height * 0.3
            }
            source: "qrc:/images/button-continue.png"
            width: parent.width * 0.22
            height: parent.width * 0.22
            onClicked: {
                continueTimer.start()
                root.state = "running" // get rid of the menues while the counter is counting down
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
            anchors {
                verticalCenter: continu.verticalCenter
                left: continu.right
                leftMargin: parent.width * 0.01
            }
            onClicked: {
                retryTimer.start()
                root.state = "running" // get rid of the menues while the counter is counting down
            }
        }

        ImageButton {
            id: levelUp
            anchors {
                left: continu.right
                top: continu.bottom
                leftMargin: -parent.width * 0.05
            }
            width: parent.width * 0.1
            height: parent.width * 0.1
            onClicked: {
                gameScene.levelUp()
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
            anchors {
                right: continu.left
                top: continu.bottom
                rightMargin: -parent.width * 0.05
            }
            width: parent.width * 0.1
            height: parent.width * 0.1

            onClicked: {
                gameScene.levelDown()
            }
            source: "qrc:/images/button-leveldown.png"
            opacity: gameScene.level > 1
            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }

        ImageButton {
            id: fullScreen
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: parent.width * 0.02
                topMargin: parent.width * 0.02
            }
            width: parent.width * 0.1
            height: parent.width * 0.1
            visible: dashboardButtonVisible
            onClicked: {
                gameScene.clickedDashboardButton()
            }
            source: "qrc:/images/button-dashboard.png"
            anchors.horizontalCenterOffset: 53
            anchors.verticalCenterOffset: 56
        }

        ImageButton {
            id: exit
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: parent.width * 0.02
                topMargin: parent.width * 0.02
            }
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
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: parent.width * 0.02
                topMargin: parent.width * 0.22
            }
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
        version: gameScene.version
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
        anchors {
            right: parent.right
            top: parent.top
            rightMargin: parent.width * 0.02
            topMargin: parent.width * 0.02
        }
        width: parent.width * 0.1
        height: parent.width * 0.1
        onClicked: {
            gameScene.gameState = GameScene.GamePaused
        }
        source: "qrc:/images/button-pause.png"
    }

    ImageButton {
        id: positiveCharges
        anchors {
            right: parent.right
            top: pause.bottom
            rightMargin: parent.width * 0.02
            topMargin: parent.height * 0.01
        }
        width: parent.width * 0.1
        height: parent.width * 0.1
        selected: true
        onClicked: {
            gameScene.selectedType = GameScene.ParticlePositive
        }
        source: "qrc:/images/button-positive-charge.png"
        selectedSource: "qrc:/images/button-positive-charge-selected.png"
    }

    ImageButton {
        id: negativeCharges
        anchors {
            right: parent.right
            top: positiveCharges.bottom
            rightMargin: parent.width * 0.02
            topMargin: parent.height * 0.01
        }
        width: parent.width * 0.1
        height: parent.width * 0.1
        selected: false
        onClicked: {
            gameScene.selectedType = GameScene.ParticleNegative
        }
        source: "qrc:/images/button-negative-charge.png"
        selectedSource: "qrc:/images/button-negative-charge-selected.png"
    }

    ImageButton {
        id: specialCharges
        anchors{
            right: parent.right
            top: negativeCharges.bottom
            rightMargin: parent.width * 0.02
            topMargin: parent.height * 0.01
        }
        width: parent.width * 0.1
        height: parent.width * 0.1
        selected: false
        visible: (gameScene.gameMode == GameScene.ModeParty && level >= 14) // only visible in party mode above level 14
        onClicked: {
            gameScene.selectedType = GameScene.ParticleSpecial
        }
        source: "qrc:/images/button-special-charge.png"
        selectedSource: "qrc:/images/button-special-charge-selected.png"
    }

    Text {
        id: timeLeft
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: parent.width * 0.03
            bottomMargin: parent.width * 0.01
        }
        text: levelTime
        color: "white"
        font.pixelSize: parent.width * 0.07
        font.family: "NovaSquare"
    }

    onRemainingNegativeChargesChanged: {
        if(remainingNegativeCharges < 1) {
            negativeCharges.source = "qrc:/images/button-empty-charge.png"
            negativeCharges.selectedSource = "qrc:/images/button-empty-charge-selected.png"
            gameScene.selectedType = GameScene.ParticlePositive
        } else {
            negativeCharges.source = "qrc:/images/button-negative-charge.png"
            negativeCharges.selectedSource = "qrc:/images/button-negative-charge-selected.png"
        }
    }

    onRemainingPositiveChargesChanged: {
        if(remainingPositiveCharges < 1) {
            positiveCharges.source = "qrc:/images/button-empty-charge.png"
            positiveCharges.selectedSource = "qrc:/images/button-empty-charge-selected.png"
            gameScene.selectedType = GameScene.ParticleNegative
        } else {
            positiveCharges.source = "qrc:/images/button-positive-charge.png"
            positiveCharges.selectedSource = "qrc:/images/button-positive-charge-selected.png"
        }
    }
    onRemainingSpecialChargesChanged: {
        if(remainingSpecialCharges < 1) {
            specialCharges.source = "qrc:/images/button-empty-charge.png"
            specialCharges.selectedSource = "qrc:/images/button-empty-charge-selected.png"
            gameScene.selectedType = GameScene.ParticlePositive
        } else {
            specialCharges.source = "qrc:/images/button-special-charge.png"
            specialCharges.selectedSource = "qrc:/images/button-special-charge-selected.png"
        }
    }

}
