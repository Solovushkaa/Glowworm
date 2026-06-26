import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
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

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowBlur: 0.5
            // shadowHorizontalOffset: 2
            // shadowVerticalOffset: 2
            shadowColor: "#20000000"
        }
    }

    // implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    // implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
    width: parent.width * 0.4
    height: parent.height * 0.2

    ColumnLayout {
        id: layout

        anchors {
            centerIn: parent
            fill: parent
            margins: 10
        }
        spacing: 10

        Text {
            Layout.alignment: Qt.AlignHCenter

            font.pointSize: 12
            text: "Quick connection key"
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

                persistentSelection: false
                selectByMouse: true
                selectByKeyboard: true

                wrapMode: TextArea.NoWrap
                clip: true

                color: "transparent"

                placeholderText: "..."
                placeholderTextColor: "#cfcfcf"

                font.pointSize: 11

                background: Rectangle {
                    radius: 3
                    border {
                        width: 1
                        color: "#dddddd"
                    }

                    Text {
                        anchors.fill: parent
                        anchors.margins: 8
                        verticalAlignment: Text.AlignVCenter

                        text: keyID.text
                        elide: Text.ElideRight
                        font: keyID.font
                        color: "black"
                        renderType: Text.NativeRendering

                        enabled: false
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
                    keyID.deselect()
                }
            }
        }
    }
}
