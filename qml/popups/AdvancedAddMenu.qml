import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CustomButtons

ColumnLayout {

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

            Layout.preferredWidth: 80

            checked: true

            indicator: Rectangle {
                width: parent.width
                height: parent.height
                radius: 20

                color: newSecureConnection.checked ? "#5371ad" : "#e0e0e0"

                Behavior on color {
                    ColorAnimation {
                        duration: 200
                        easing.type: Easing.InOutQuad
                    }
                }

                Text {
                    visible: newSecureConnection.checked

                    text: "ON"
                    font.bold: true
                    font.pixelSize: 12
                    color: "white"

                    anchors {
                        left: parent.left
                        leftMargin: 12
                        verticalCenter: parent.verticalCenter
                    }

                    Behavior on color {
                        ColorAnimation {
                            duration: 200
                        }
                    }
                }
                Text {
                    visible: !newSecureConnection.checked

                    text: "OFF"
                    font.bold: true
                    font.pixelSize: 12
                    color: "#757575"

                    anchors {
                        right: parent.right
                        rightMargin: 12
                        verticalCenter: parent.verticalCenter
                    }

                    Behavior on color {
                        ColorAnimation {
                            duration: 200
                        }
                    }
                }

                Rectangle {
                    property int ssize: parent.height - 4

                    width: ssize
                    height: ssize
                    radius: ssize / 2
                    y: 2

                    color: "white"

                    x: newSecureConnection.checked ? parent.width - width - 2 : 2

                    Behavior on x {
                        NumberAnimation {
                            duration: 200
                            easing.type: Easing.OutCubic
                        }
                    }
                }
            }

            contentItem: Text {
                text: newSecureConnection.text
                font.pointSize: 11
                leftPadding: newSecureConnection.indicator.width + 8
                verticalAlignment: Text.AlignVCenter
                visible: newSecureConnection.text !== ""
            }
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
