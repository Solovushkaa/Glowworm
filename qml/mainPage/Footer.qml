import QtQuick

Rectangle {
        id: root
        anchors{
            right: parent.right
            left: parent.left
        }
        height: 20

        property string footerText: "..."

        Rectangle {
            anchors{
                bottom: parent.top
                right: parent.right
                left: parent.left
            }

            height: 2
            color: "#cccccc"
        }

        Text {
            font.pointSize: 10
            anchors{
                // verticalCenter: parent
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 10
            }

            color: "#3f3f3f"

            text: footerText
        }
    }
