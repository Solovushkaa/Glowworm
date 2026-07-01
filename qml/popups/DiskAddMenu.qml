import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


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
            text: "<font color='red'>*</font>Address: "
        }
        TextArea {
            id: newAddress

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
            text: "<font color='red'>*</font>Port: "
        }
        TextArea {
            id: newPort

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
            text: "<font color='red'>*</font>Username: "
        }
        TextArea {
            id: newWebDavUsername

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
            text: "<font color='red'>*</font>Password: "
        }
        TextArea {
            id: newWebDavPassword

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

    CustomButton {
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: 100
        Layout.preferredHeight: 40

        buttonText: "Add"

        onClicked: {

            ClientConnectionManager.addWebDavConnection(newAddress.text,
                                                        newConnectionName.text,
                                                        newWebDavUsername.text,
                                                        newWebDavPassword.text,
                                                        parseInt(newPort.text),
                                                        isPersistentConnection)

            addPopup.close()

            newAddress.text = ""
            newPort.text = ""
            newConnectionName.text = ""
            newWebDavUsername.text = ""
            newWebDavPassword.text = ""
        }
    }
    Item {
        Layout.fillHeight: true
    }
}
