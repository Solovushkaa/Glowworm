import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Rectangle {
    color: "transparent"
    height: 22

    property string activeConnectionName: ""

    Connections {
        target: ClientConnectionManager
        function onActiveConnectionChanged() {
            activeConnectionName = ClientConnectionManager.getActiveConnection(
                        ) !== null ? ClientConnectionManager.getActiveConnection(
                                         ).name : ""
        }
    }

    Rectangle {
        color: "transparent"
        anchors {
            left: parent.left
            leftMargin: 20
            top: parent.top
            topMargin: 30
            bottom: parent.bottom
        }

        width: 300

        Label {
            anchors {
                verticalCenter: parent.verticalCenter
            }

            font.pointSize: 12
            color: "black"
            text: activeConnectionName
        }
    }

    Rectangle {
        id: headerConnectionState
        color: "#efefef"
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        topLeftRadius: 6
        bottomLeftRadius: 6
        bottomRightRadius: 6

        // radius: 6
        border {
            width: 1
            color: "#cccccc"
        }

        width: 30

        Rectangle {
            id: connLight
            width: 14
            height: 14
            radius: width / 2
            // color: isPageInteractiveActive ? "#5371ad" : "#a5a5a5"
            color: isPageInteractiveActive ? "#75e075" : "#a5a5a5"
            anchors {
                centerIn: parent
            }

            border {
                width: 1
                color: "#dddddd"
            }

            // Shadow
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowBlur: 0.5
                shadowHorizontalOffset: 2
                shadowVerticalOffset: 2
                shadowColor: "#20000000"
            }
        }
    }
}
