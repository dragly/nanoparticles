//import QtQuick 1.0
// must be Qt 4.7 for Maemo
import Qt 4.7

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
            text: "Nanoparticles v1.99.1"
            anchors.left: parent.left
            anchors.top: parent.top
            font.pixelSize: 0.09 * root.height
            wrapMode: Text.NoWrap
        }

        Text {
            id: text2
            anchors.top: text1.bottom
            color: "#ffffff"
            text: "Physics based game by dragly.<br><br><b>How do I play this game?</b><br>You control the green charge. Avoid hitting the purple charges by placing blue and red charges on the map. The blue charges attract the green charge, while the red repel it.<br>In the lower right corner you see the time left. When the time runs out, you have completed the level.<br><i>Hint:</i> Use the blue charges with caution. Whenever the green charge bumps into one of the other charges, it gets stronger. The stronger it gets, the harder it is to control.<br><br><b>Are the physics behind this game real?</b><br>To a very good approximation they imitate the physics of real world particles. Behind it all is the Euler-Cromer method, Coulomb's law and a good dose of Newtonian physics. However, some adjustments have been made for the sake of entertainment.<br><br><i>Do not use this game as a simulator in safety-critical systems.</i><br>(CERN scientists, I'm looking at you...)<br><br><b>Legal info</b><br>This application is licensed under the GPL license v3 or newer.<br><br>Please visit <a href='http://dragly.org/source/nanoparticles'>dragly.org</a> for more information.<br><br>The font used in this application is named Nova Square and created by Wojciech Kalinowski &lt;wmk69@o2.pl&gt;, with no affiliation with the application itself.<br>The font is licensed under the SIL Open Font License 1.1."
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
        width: 0.1 * root.width
        height: 0.1 * root.width
        anchors.rightMargin: 0.02 * root.width
        anchors.topMargin: 0.02 * root.width
        source: "qrc:/images/button-exit.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {root.opacity = 0}
        }

    }
}
