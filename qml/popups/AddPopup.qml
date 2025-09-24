import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CustomButtons
import SavedConnectionManager

Popup {
    id: root

    anchors {
        centerIn: parent
    }

    width: parent.width * 0.4
    height: parent.height * 0.42

    modal: true
    focus: true

    closePolicy: Popup.CloseOnEscape

    background: Rectangle {
        color: "white"

        radius: 30
    }

    ColumnLayout {
        anchors {
            fill: parent
            margins: 10
        }

        spacing: 10

        Text {
            Layout.alignment: Qt.AlignHCenter

            font.pointSize: 12
            text: "Add Connection"
        }
        RowLayout {
            Layout.fillWidth: true

            Text {
                font.pointSize: 11
                text: "name: "
            }
            TextArea {
                id: newPresetName

                Layout.fillWidth: true

                placeholderText: "..."

                color: "black"
                font.pointSize: 11

                background: Rectangle {
                    radius: 3

                    border {
                        width: 1
                        color: "#dddddd"
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true

            Text {
                font.pointSize: 11
                text: "ip: "
            }
            TextArea {
                id: newPresetIp

                Layout.fillWidth: true

                placeholderText: "..."

                color: "black"
                font.pointSize: 11

                background: Rectangle {
                    radius: 3

                    border {
                        width: 1
                        color: "#dddddd"
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true

            Text {
                font.pointSize: 11
                text: "port: "
            }
            TextArea {
                id: newPresetPort

                Layout.fillWidth: true

                placeholderText: "..."

                color: "black"
                font.pointSize: 11

                background: Rectangle {
                    radius: 3

                    border {
                        width: 1
                        color: "#dddddd"
                    }
                }
            }
        }
        CustomButton {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 100
            Layout.preferredHeight: 40

            buttonText: "Add"

            onClicked: {

                ConnectionManager.add(newPresetName.text, "HTTP",
                                      newPresetIp.text, newPresetPort.text)
                startAddPopup.close()
                root.close()
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
