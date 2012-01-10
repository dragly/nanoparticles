import QtQuick 1.0

ImageButton {
    property bool selected
    property string normalSource
    property string selectedSource

    source: normalSource

    function refreshImage() {
        if(selected) {
            source = selectedSource
        } else {
            source = normalSource
        }
    }

    onSelectedChanged: {
        refreshImage()
    }
    onNormalSourceChanged: {
        refreshImage()
    }
    onSelectedSourceChanged: {
        refreshImage()
    }

    Image {
        id: selectedImage
        source: parent.selectedSource
        anchors.fill: parent
        opacity: 0
    }

    selected: false
}
