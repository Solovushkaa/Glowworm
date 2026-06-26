import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

Button {
    id: internal_button

    anchors {
        top: parent.top
        left: parent.left
        margins: 5
    }
    width: 40
    height: 40

    property int borderWidth: 1

    background: Rectangle {
        radius: 4

        // border {
        //     width: internal_button.borderWidth
        //     color: "#cccccc"
        // }
        // layer.enabled: true
        // layer.effect: MultiEffect {
        //     shadowEnabled: true
        //     shadowBlur: 0.5
        //     // shadowHorizontalOffset: 2
        //     // shadowVerticalOffset: 2
        //     shadowColor: "#20000000"
        // }
    }

    ColumnLayout {
        anchors {
            fill: parent
            margins: 8
        }

        spacing: 3
        Rectangle {
            color: "#cfcfcf"
            Layout.fillWidth: parent
            height: 4
            radius: 4
        }
        Rectangle {
            color: "#cfcfcf"
            Layout.fillWidth: parent
            height: 4
            radius: 4
        }
        Rectangle {
            color: "#cfcfcf"
            Layout.fillWidth: parent
            height: 4
            radius: 4
        }
    }
}
