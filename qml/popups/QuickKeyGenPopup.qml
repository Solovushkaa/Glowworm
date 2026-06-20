import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CustomButtons

Popup {
    id: root

    function getTextOrPlaceholder(textArea) {
        return parseInt(
                    textArea.text !== "" ? textArea.text : textArea.placeholderText)
    }

    anchors {
        centerIn: parent
    }

    // width: parent.width * 0.4
    // height: parent.height * 0.42
    padding: 10

    modal: true
    focus: true

    closePolicy: Popup.CloseOnEscape

    background: Rectangle {
        color: "white"

        radius: 30
    }

    // implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    // implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
    width: parent.width * 0.4
    implicitHeight: layout.implicitHeight + topPadding + bottomPadding

    ColumnLayout {
        id: layout

        // anchors.centerIn: parent
        anchors {
            fill: parent
            margins: 10
        }
        spacing: 10

        Text {
            Layout.alignment: Qt.AlignHCenter

            font.pointSize: 12
            text: "Generate quick connection key"
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            spacing: 10
            Text {
                id: textKeyID
                Layout.alignment: Qt.AlignLeft
                font.pointSize: 12

                text: "Key:"
            }
            TextArea {
                id: keyID

                Layout.fillWidth: true

                text: Server.generateConnectionKey()

                placeholderText: "..."
                placeholderTextColor: "#cfcfcf"

                color: "black"
                font.pointSize: 11

                background: Rectangle {
                    radius: 3

                    border {
                        width: 1
                        color: "#dddddd"
                    }
                }

                readOnly: true
            }
            CustomButton {
                id: copyButton

                Layout.preferredHeight: parent.height * 0.9

                buttonText: "Copy"
                onClicked: {
                    keyID.selectAll()
                    keyID.copy()
                }
            }
        }
    }
}
