import QtQuick 1.0

Rectangle {

    id: root
    width: 400
    height: 300
    color: "#000000"
    opacity: 1.0

    Behavior on opacity {
        NumberAnimation { duration: 500 }
    }

    MouseArea {
        id: mouse_area1
        anchors.fill: parent
        onClicked: {console.log(parent.width)}
    }

    Flickable {
        id: textArea
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 0.8 * parent.width
        anchors.topMargin: 0.05 * root.height
        anchors.leftMargin: 0.05 * root.height
        contentHeight: text1.height + text2.height + text2.anchors.topMargin
        Text {
            id: text1
            color: "#ffffff"
            text: "Nanoparticles v0.5.0"
            anchors.left: parent.left
            anchors.top: parent.top
            font.pixelSize: 0.09 * root.height
            wrapMode: Text.NoWrap
        }

        Text {
            id: text2
            anchors.top: text1.bottom
            color: "#ffffff"
            text: "Physics based game by dragly.<br><br>This application is licensed under the GPL license v3 or newer.<br><br>Please visit <a href='http://dragly.org/source/nanoparticles'>dragly.org</a> for more information.<br><br>The font used in this application is named Nova Square and created by Wojciech Kalinowski &lt;wmk69@o2.pl&gt;, with no affiliation with the application itself.<br>The font is licensed under the SIL Open Font License 1.1."
            anchors.left: parent.left
            anchors.topMargin: 0.02 * root.height
            anchors.right: parent.right
            wrapMode: Text.WordWrap
            font.pixelSize: 0.065 * root.height
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }

    Image {
        anchors.right:  parent.right
        anchors.top: parent.top

        id: image1
        width: 0.12 * parent.width
        height: 0.12 * parent.width
        anchors.rightMargin: 0.02 * root.height
        anchors.topMargin: 0.02 * root.height
        source: "qrc:/images/button-exit.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {root.opacity = 0}
        }

    }
}
