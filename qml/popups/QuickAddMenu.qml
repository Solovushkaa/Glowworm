import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


ColumnLayout {

    RowLayout {
        Layout.fillWidth: true

        Text {
            textFormat: Text.RichText
            font.pointSize: 11
            text: "<font color='red'></font>Name: "
        }
        TextArea {
            id: newQuickConnectionName

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
            text: "<font color='red'>*</font>Key: "
        }
        TextArea {
            id: newQuickConnection

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

            ClientConnectionManager.addQuickConnection(newQuickConnection.text,
                                                       isPersistentConnection)

            // startAddPopup.close()
            addPopup.close()

            newQuickConnection.text = ""
        }
    }
    Item {
        Layout.fillHeight: true
    }
}
