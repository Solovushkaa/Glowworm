import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import HttpClient
import CustomButtons

Popup {
    id: root

    property string headerText: ""
    property bool isDownload: false
    property string savePath: ""

    anchors {
        centerIn: parent
    }

    width: Screen.width * 0.13
    height: Screen.height * 0.15

    focus: true
    modal: true

    background: Rectangle {
        anchors.fill: parent
        radius: 8
        color: "white"
    }

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onClosed: {
        if (isDownload) {
            Client.getFile(downloadPath, savePath, currentName)
            isDownload = false
            downloadPath = ""
            savePath = ""
        }
    }

    ColumnLayout {

        spacing: 10
        anchors {
            margins: 10
            fill: parent
        }

        Text {
            id: downloadAttention

            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true


            color: "black"
            font.bold: true
            font.pointSize: 11


            clip: true
            wrapMode: Text.Wrap
            maximumLineCount: 2
            elide: Text.ElideRight

            text: headerText
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            ScrollView {

                width: 130
                height: 30

                TextArea {
                    id: selectSaveFolderTextArea

                    color: "black"
                    font.bold: true
                    font.pixelSize: 14
                    wrapMode: TextArea.NoWrap

                    // clip: true
                    implicitWidth: width
                    implicitHeight: height

                    text: savePath

                    horizontalAlignment: Text.AlignLeft

                    background: Rectangle {
                        radius: 8
                        border.color: "lightgrey"
                        border.width: 2
                    }
                }
            }

            CustomButton {
                id: selectSaveFolderButton

                buttonText: "Select"

                Layout.preferredWidth: 80
                Layout.preferredHeight: 30

                onClicked: {
                    console.log("мауу: ")
                    folderDialog.open()
                }
            }

            FolderDialog {
                id: folderDialog
                currentFolder: "file:///"
                onAccepted: {
                    savePath = folderDialog.selectedFolder.toString(
                                ).replace("file://", "") + "/"
                    console.log("Выбрана эта директория: ", savePath)
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            CustomButton {
                id: acceptButton

                buttonText: "Download"

                Layout.preferredWidth: 100
                Layout.preferredHeight: 40

                onClicked: {
                    console.log("Подтверждение скачивания: ",
                                currentName)
                    if (savePath !== "") {
                        isDownload = true
                        root.close()
                    }
                }
            }
            CustomButton {
                id: cancelButton

                buttonText: "Cancel"
                Layout.preferredWidth: 100
                Layout.preferredHeight: 40

                onClicked: {
                    console.log("Отмена скачивания: ", currentName)
                    isDownload = false
                    root.close()
                }
            }
        }
    }
}
