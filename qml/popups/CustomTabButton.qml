import QtQuick
import QtQuick.Controls

TabButton {
    id: root
    property string buttonText: ""
    property int radius: height / 2
    property int durationTime: 200
    property int customMargin: parent.height - parent.height * 0.8

    text: buttonText

    height: parent.height
    anchors {
        top: parent.top
    }
    contentItem: Text {
        text: parent.text
        font.pointSize: 10

        font.weight: parent.checked ? Font.DemiBold : Font.Normal

        color: parent.checked ? "#5371ad" : "black"

        renderType: Text.NativeRendering

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        Behavior on color {
            ColorAnimation {
                duration: root.durationTime
                easing.type: Easing.OutCubic
            }
        }
        Behavior on font.weight {
            NumberAnimation {
                duration: root.durationTime
                easing.type: Easing.OutCubic
            }
        }
    }

    background: Rectangle {
        anchors.centerIn: parent
        width: parent.checked ? parent.width - root.customMargin : parent.width * 0.6
        height: parent.checked ? parent.height - root.customMargin : parent.height * 0.6

        color: parent.checked ? "#f5f7fc" : "transparent"
        radius: parent.height / 2

        Behavior on height {
            NumberAnimation {
                duration: root.durationTime
                easing.type: Easing.OutCubic
            }
        }
        Behavior on width {
            NumberAnimation {
                duration: root.durationTime
                easing.type: Easing.OutCubic
            }
        }
    }
}
