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
            text: "<font color='red'>*</font>Url: "
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
                                                        isPersistentConnection)

            addPopup.close()

            newAddress.text = ""
            newConnectionName.text = ""
            newWebDavUsername.text = ""
            newWebDavPassword.text = ""
        }
    }
    Item {
        Layout.fillHeight: true
    }
}
