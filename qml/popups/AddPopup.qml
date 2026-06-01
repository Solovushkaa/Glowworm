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
            text: "Add Connection"
        }
        RowLayout {
            Layout.fillWidth: true

            Text {
                textFormat: Text.RichText
                font.pointSize: 11
                text: "<font color='red'>*</font>Name: "
            }
            TextArea {
                id: newConnectionName

                Layout.fillWidth: true

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
            }
        }
        RowLayout {
            Layout.fillWidth: true

            Text {
                textFormat: Text.RichText
                font.pointSize: 11
                text: "<font color='red'>*</font>Connection type: "
            }
            TextArea {
                id: newConnectionType

                Layout.fillWidth: true

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
            }
        }
        RowLayout {
            Layout.fillWidth: true

            Text {
                textFormat: Text.RichText
                font.pointSize: 11
                text: "<font color='red'>*</font>Address: "
            }
            TextArea {
                id: newConnectionURL

                Layout.fillWidth: true

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
            }
        }
        //===================================
        RowLayout {
            Layout.fillWidth: true

            Text {
                textFormat: Text.RichText
                font.pointSize: 11
                text: "Secure connection: "
            }
            Switch {
                id: newSecureConnection

                checked: true
            }
        }
        //===================================
        RowLayout {
            Layout.fillWidth: true

            Text {
                textFormat: Text.RichText
                font.pointSize: 11
                text: "Default messenger port: "
            }
            TextArea {
                id: newDefaultMessengerPort

                Layout.fillWidth: true

                placeholderText: "6115"
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
            }
        }
        //===================================
        RowLayout {
            Layout.fillWidth: true

            Text {
                textFormat: Text.RichText
                font.pointSize: 11
                text: "Secure messenger port: "
            }
            TextArea {
                id: newSecureMessengerPort

                Layout.fillWidth: true

                placeholderText: "274"
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
            }
        }
        //===================================
        RowLayout {
            Layout.fillWidth: true

            Text {
                textFormat: Text.RichText
                font.pointSize: 11
                text: "Default transport port: "
            }
            TextArea {
                id: newDefaultTransportPort

                Layout.fillWidth: true

                placeholderText: "6821"
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
            }
        }
        //===================================
        RowLayout {
            Layout.fillWidth: true

            Text {
                textFormat: Text.RichText
                font.pointSize: 11
                text: "Secure transport port: "
            }
            TextArea {
                id: newSecureTransportPort

                Layout.fillWidth: true

                placeholderText: "13119"
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
            }
        }
        CustomButton {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 100
            Layout.preferredHeight: 40

            buttonText: "Add"

            onClicked: {

                ClientConnectionManager.addConnection(
                            newConnectionName.text, parseInt(
                                newConnectionType.text), newConnectionURL.text,
                            "user", newSecureConnection.checked,
                            getTextOrPlaceholder(newDefaultMessengerPort),
                            getTextOrPlaceholder(newSecureMessengerPort),
                            getTextOrPlaceholder(newDefaultTransportPort),
                            getTextOrPlaceholder(newSecureTransportPort))

                // startAddPopup.close()
                addPopup.close()

                newConnectionName.text = ""
                newConnectionType.text = ""
                newConnectionURL.text = ""
                newSecureConnection.checked = true
                newDefaultMessengerPort.text = ""
                newSecureMessengerPort.text = ""
                newDefaultTransportPort.text = ""
                newSecureTransportPort.text = ""
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
