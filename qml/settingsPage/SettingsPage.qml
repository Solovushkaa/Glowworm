import QtQuick
import QtQuick.Controls
import CustomButtons

Page {
    anchors.fill: parent
    Rectangle {
        anchors.fill: parent

        color: "white"

        Button {
            anchors {
                left: parent.left
                leftMargin: 5
                top: parent.top
                topMargin: 5
            }

            height: 40
            width: 40

            background: Image {
                anchors {
                    fill: parent
                    margins: 10
                }

                source: "qrc:/Content/Icons/close.svg"

                fillMode: Image.PreserveAspectFit
            }

            onClicked: {
                pagesStack.pop()
            }
        }
    }
}
