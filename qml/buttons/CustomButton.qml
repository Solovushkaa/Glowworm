import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Universal
import QtQuick.Layouts

Button {
    id: customButton
    Layout.preferredWidth: name.width + 20
    Layout.preferredHeight: 44

    font.pointSize: 11
    font.bold: true

    property string buttonText: "Default"
    property bool isActive: true

    Text {
        id: name
        anchors.centerIn: parent
        font.pixelSize: 14
        font.bold: true
        color: isActive ? (customButton.pressed ? "#dee9ff" : "#ffffff") : "#fefefe"
        text: buttonText
    }

    background: Rectangle {
        radius: 8
        color: isActive ? (customButton.pressed ? "#3d5482" : (customButton.hovered ? "#5675b3" : "#5371ad")) : "#a5a5a5"
    }

    MouseArea {
        anchors.fill: parent
        visible: !isActive
    }
}
