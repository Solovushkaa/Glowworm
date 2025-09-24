import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {

    anchors {
        centerIn: parent
    }

    width: parent.width * 0.9
    height: parent.height * 0.9

    modal: true
    focus: true

    background: Rectangle {
        color: "white"

        radius: 25

        MouseArea {
            anchors.fill: parent

            onClicked: {
                addPopup.open()
            }
        }
    }

    closePolicy: Popup.NoAutoClose

    ColumnLayout {
        anchors {
            fill: parent
        }

        Item {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignCenter
        }
        Text {
            text: "Add connection"
            Layout.alignment: Qt.AlignCenter
        }
        Image {
            source: "qrc:/Content/Icons/plus.svg"
            Layout.preferredHeight: 40
            Layout.preferredWidth: 40
            Layout.alignment: Qt.AlignCenter

            smooth: true
            antialiasing: true
            mipmap: true

            fillMode: Image.PreserveAspectFit
        }
        Item {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignCenter
        }
    }
}
