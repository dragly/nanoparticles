import QtQuick 1.0

ImageButton {
    property bool selected
    onSelectedChanged: {
        if(selected) {
            selectedImage.opacity = 1.0
        } else {
            selectedImage.opacity = 0.0
        }
    }
    selected: false

    Image {
        id: selectedImage
        source: "qrc:/images/selection-overlay.png"
        anchors.fill: parent
        opacity: 0
    }

//    Text {
//        anchors.centerIn: parent
//        text: "12"
//    }
}
