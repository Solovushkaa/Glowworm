import QtQuick
import QtQuick.Effects
import QtQuick.Controls

ToolTip {
    id: tooltip

    property var visibilityController: null
    property bool visibilityMode: true

    property double saveMouseX: !tooltip.visible ? visibilityController.mouseX : saveMouseX
    property double saveMouseY: !tooltip.visible ? visibilityController.mouseY : saveMouseY

    visible: visibilityMode /*&& tooltipMouseArea.containsMouse*/
    opacity: tooltip.visible ? 1 : 0

    delay: 350
    timeout: 0

    x: saveMouseX + 15
    y: saveMouseY + 5

    width: Screen.width * 0.08
    height: Screen.height * 0.15

    background: Rectangle {
        color: "#ffffff"
        radius: 4

        opacity: tooltip.visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowBlur: 0.5
            // shadowHorizontalOffset: 2
            // shadowVerticalOffset: 2
            shadowColor: "#20000000"
        }
    }

    MouseArea {
        id: tooltipMouseArea
        anchors.fill: parent
        hoverEnabled: true
    }
}
