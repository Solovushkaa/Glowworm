import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

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

    closePolicy: Popup.CloseOnEscape

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
            font.pointSize: 16
            renderTypeQuality: Text.HighRenderTypeQuality
            renderType: Text.NativeRendering
        }
        Rectangle {
            Layout.alignment: Qt.AlignCenter

            Layout.preferredWidth: 50
            Layout.preferredHeight: 50

            Image {
                id: plus

                anchors.fill: parent

                source: "qrc:Icons/plus.svg"
                sourceSize.width: 300
                sourceSize.height: 300

                visible: false

                // smooth: true
                antialiasing: true
                mipmap: true
                fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
                anchors.fill: plus
                source: plus
                color: "#5371ad"
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignCenter
        }
    }
}
