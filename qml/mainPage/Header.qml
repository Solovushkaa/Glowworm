import QtQuick
import QtQuick.Controls
import DirectClient

Rectangle {
    color: "transparent"
    height: 22

    property string m_activeConnection: ""

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
            text: "Active: " + m_activeConnection
        }
    }

    Rectangle {
        color: "#efefef"
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        radius: 6

        border {
            width: 1
            color: "#cccccc"
        }

        width: isPageInteractiveActive ? 100 : 110

        Rectangle {
            width: 10
            height: 10
            radius: width/2
            color: isPageInteractiveActive ? "#009e05" : "gray"
            anchors {
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }
        }

        Text {
            anchors {
                fill: parent
                rightMargin: 5
            }
            font.pointSize: 10
            color: "black"
            text: isPageInteractiveActive ? "Connected" : "Disconnected"

            horizontalAlignment: Text.AlignRight
        }
    }
}
