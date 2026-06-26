import QtQuick
import Qt5Compat.GraphicalEffects

Item {
        property string source: ""
        property string color: ""

        Image {
            id: image

            anchors.fill: parent

            source: parent.source

            visible: false

            smooth: true
            antialiasing: true
            mipmap: true

            fillMode: Image.PreserveAspectFit
        }
        ColorOverlay {
            anchors.fill: image
            source: image
            color: parent.color
        }
}
