import QtQuick
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root
    property double value: 0

    color: "white"

    radius: 3
    border {
        width: 1
        color: "lightgrey"
    }

    Text {
        id: progressBarTextBlack
        anchors.centerIn: parent

        textFormat: Text.RichText

        text: "<font size = 4>" + Math.round(parent.value * 10000) / 100 + "</font><font size=3>%</font>"

        color: "black"
        visible: true
    }

    Rectangle {
        id: progress
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: parent.width * value

        radius: parent.radius

        color: "#009e05"
    }

    Item {
        x: progress.x
        y: progress.y

        width: progress.width
        height: progress.height

        clip: true
        Text {
            id: progressBarTextWhite
            x: progressBarTextBlack.x
            y: progressBarTextBlack.y

            textFormat: Text.RichText

            text: progressBarTextBlack.text

            color: "white"
        }
    }
}
