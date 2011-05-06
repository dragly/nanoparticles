import QtQuick 1.0

Rectangle {
    property real realwidth: 400
    property real realheight: 300

    id: root
    width: 400
    height: 300
    color: "#000000"
    opacity: 1.0

    Behavior on opacity {
        NumberAnimation { duration: 500 }
    }

    Text {
        id: text1
        x: 15
        y: 15
        width: 80
        height: 20
        color: "#ffffff"
        text: "Reaktor v1.0.0"
        font.pixelSize: 16 * root.height / root.realheight
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.top: parent.top
        anchors.topMargin: 15 * root.height / root.realheight
        wrapMode: Text.NoWrap
        styleColor: "#ffffff"
    }

    Text {
        id: text2
        x: 15
        y: 43
        width: 150
        color: "#ffffff"
        text: "Physics based game by dragly."
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.top: parent.top
        anchors.topMargin: 43 * root.height / root.realheight
        wrapMode: Text.NoWrap
        font.pixelSize: 12 * root.height / root.realheight
    }

    Text {
        id: text4
        width: 150
        color: "#ffffff"
        text: "This game is open source. Check out http://dragly.com for more information."
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.top: parent.top
        anchors.topMargin: 88 * root.height / root.realheight
        wrapMode: Text.NoWrap
        font.pixelSize: 12 * root.height / root.realheight
    }

    Image {
        id: image1
        x: 282
        y: 15
        width: 100
        height: 100
        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.right: parent.right
        anchors.rightMargin: 18
        source: "qrc:/images/button-exit.png"
        MouseArea {
            onClicked: {root.opacity = 0}
            anchors.fill: parent
        }

    }
}
