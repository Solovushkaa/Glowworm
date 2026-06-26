import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CustomButtons
import QtCore

Popup {
    id: root

    property string headerText: ""
    property string savePath: ""
    property string standardDownloadsPath: StandardPaths.writableLocation(
                                               StandardPaths.DownloadLocation)
    property bool isCustomSavePath: false

    anchors {
        centerIn: parent
    }

    width: Screen.width * 0.17
    height: Screen.height * 0.2

    focus: true
    modal: true

    background: Rectangle {
        anchors.fill: parent
        radius: 8
        color: "white"
    }

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

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

            text: root.headerText
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            ScrollView {

                width: 170
                height: 40

                TextArea {
                    id: selectSaveFolderTextArea

                    color: "black"
                    // font.bold: true
                    font.pixelSize: 14
                    wrapMode: TextArea.NoWrap

                    // clip: true
                    implicitWidth: width
                    implicitHeight: height

                    text: {
                        if (!root.isCustomSavePath) {
                            return (root.standardDownloadsPath + "/" + root.headerText).replace(
                                        "file://", "")
                        } else {
                            return savePath
                        }
                    }

                    horizontalAlignment: Text.AlignLeft

                    topPadding: (height - contentHeight) / 2
                    bottomPadding: (height - contentHeight) / 2

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
                Layout.preferredHeight: 40

                onClicked: {
                    console.log("FolderDialog open")
                    folderDialog.open()
                }
            }

            FolderDialog {
                id: folderDialog
                currentFolder: "file:///"
                onAccepted: {
                    savePath = folderDialog.selectedFolder.toString().replace(
                                "file://", "") + "/" + root.headerText

                    isCustomSavePath = true

                    console.log("Selected directory:", savePath)
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
                    console.log("Download verification:",
                                selectSaveFolderTextArea.text)
                    if (selectSaveFolderTextArea.text !== "") {
                        Client.getFile(fileIndex, currentName,
                                       selectSaveFolderTextArea.text)

                        root.isCustomSavePath = false
                        // Client.getFileFromRelayServer(
                        //             fileIndex, "user", currentName,
                        //             selectSaveFolderTextArea.text)
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
                    console.log("Download canceled:", currentName)
                    root.close()
                }
            }
        }
    }
}
