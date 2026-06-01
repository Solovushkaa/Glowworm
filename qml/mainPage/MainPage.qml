import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CustomButtons

Page {
    id: mainPage
    anchors.fill: parent

    background: Rectangle {
        color: "#eeeeee"
    }

    property ApplicationWindow mainWindow: null

    property string currentPath: "/"
    property string currentName: ""
    property int currentSize: 0
    property int fileIndex: -1
    property bool isPageInteractiveActive: false

    Connections {
        target: Client
        function onConnectionStatusCodeChanged(currentStatusCode) {
            if (currentStatusCode === 200)
                mainPage.isPageInteractiveActive = true
        }
    }

    function updateFileInfoPanel(name, type, location, size, created, modified, accessed) {
        fileInfo.name = name
        fileInfo.typeName = type
        fileInfo.locationName = location
        fileInfo.sizeName = size
        fileInfo.createdName = created
        fileInfo.modifiedName = modified
        fileInfo.accessedName = accessed
    }

    BurgerMenu {
        id: burgerMenu
        mainWindow: mainPage.mainWindow
    }
    Item {
        id: burgerCollision
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }

        width: 50
    }

    Header {
        id: mainPageHeader

        anchors {
            left: burgerCollision.right
            top: parent.top
            right: parent.right
        }
    }

    footer: Footer {
        id: mainFooter
    }

    ColumnLayout {
        id: contentArea
        anchors {
            top: parent.top
            left: burgerCollision.right
            right: parent.right
            leftMargin: 20
            bottom: parent.bottom
        }

        spacing: 20

        RowLayout {
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.topMargin: 40

            spacing: 20

            Rectangle {
                id: fileBrowser
                Layout.preferredWidth: Window.width * 0.7
                Layout.minimumWidth: 100
                Layout.preferredHeight: Window.height * 0.6
                Layout.minimumHeight: 250

                radius: 4
                color: "white"

                ListView {
                    id: listView
                    anchors.fill: parent
                    anchors.margins: 5

                    highlightMoveDuration: 0
                    highlightResizeDuration: 0

                    focus: true

                    clip: true

                    boundsBehavior: Flickable.StopAtBounds

                    currentIndex: -1

                    model: DirectoryManager

                    delegate: Rectangle {
                        id: click
                        width: fileBrowser.width - 10
                        height: 20
                        focus: true
                        color: ListView.isCurrentItem ? (listView.activeFocus ? "lightgrey" : "#e9e9e9") : "white"

                        Text {
                            anchors.left: parent.left
                            text: model.isReadable ? (model.isDir ? "📁" : "📄") : "🔒"
                            font.pixelSize: 14
                            renderType: Text.NativeRendering
                        }

                        Text {
                            anchors.left: parent.left
                            anchors.margins: 20
                            text: model.name
                            font.pixelSize: 14
                        }

                        Text {
                            anchors.right: parent.right
                            text: model.isDir ? "" : model.size.toString()
                            font.pixelSize: 14
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                listView.currentIndex = index
                                console.log("Выбран элемент:", model.name)
                                currentName = model.name
                                currentSize = model.size
                                fileIndex = listView.currentIndex
                                listView.forceActiveFocus()

                                updateFileInfoPanel(
                                            model.name,
                                            model.isDir ? "Directory" : "File",
                                            model.path,
                                            model.isDir ? "" : model.size,
                                            model.created, model.modified,
                                            model.accessed)
                            }
                            onDoubleClicked: {
                                listView.currentIndex = index
                                console.log("Его путь:", model.path)
                                listView.forceActiveFocus()

                                if (model.isDir && model.isReadable) {
                                    currentPath = model.path
                                    Client.getDirectory(currentPath)
                                    fileIndex = -1
                                }
                            }
                        }
                    }
                    ScrollBar.vertical: ScrollBar {
                        id: verticalScrollBar
                        parent: listView.parent
                        anchors.top: listView.top
                        anchors.left: listView.right
                        anchors.bottom: listView.bottom
                        anchors.leftMargin: 2
                    }
                }

                Rectangle {
                    anchors {
                        top: fileBrowser.bottom
                        left: fileBrowser.left
                        right: fileBrowser.right
                    }
                    height: 54
                    color: "transparent"

                    Rectangle {
                        anchors {
                            top: parent.top
                            topMargin: -2
                            left: parent.left
                            right: parent.right
                        }

                        height: 4

                        z: 10

                        color: "#bfbfbf"
                    }

                    Rectangle {
                        anchors {
                            top: parent.top
                            left: parent.left
                            right: parent.right
                        }

                        height: 10

                        color: "#cfcfcf"
                    }
                    Rectangle {
                        anchors {
                            top: parent.top
                            left: parent.left
                            right: parent.right
                        }

                        height: 54

                        radius: 8

                        color: "#cfcfcf"
                    }

                    RowLayout {
                        spacing: 10

                        anchors {
                            fill: parent
                            leftMargin: 5
                        }

                        CustomButton {
                            id: connectionButton

                            isActive: ClientConnectionManager.hasActiveConnection
                            buttonText: "Connect"
                            onClicked: {
                                Client.checkConnectionToServer()
                            }
                        }
                        CustomButton {
                            id: getDirectoryButton

                            isActive: isPageInteractiveActive
                            buttonText: "Get directory"
                            onClicked: {
                                Client.getDirectory(currentPath)
                            }
                        }
                        CustomButton {
                            id: downloadContentButton

                            isActive: mainPage.isPageInteractiveActive
                            buttonText: "Download"
                            onClicked: {
                                if (fileIndex != -1) {
                                    downloadMenu.open()
                                } else {
                                    console.log("Download error")
                                }
                            }
                        }
                        CustomButton {
                            id: startServer

                            buttonText: "Start server"
                            onClicked: {
                                Server.startDefaultServer()
                            }
                        }
                        CustomButton {
                            id: connectToRelayServer

                            buttonText: "Connect relay server"
                            onClicked: {
                                Client.connectToRelayServer()
                            }
                        }
                        CustomButton {
                            id: downloadFromRelayServer

                            buttonText: "Dw Rserv"
                            onClicked: {

                                Client.getFileFromRelayServer()
                            }
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                    }
                }
            }

            FileInfoPanel {
                id: fileInfo

                Layout.fillWidth: true
                Layout.rightMargin: 20
                Layout.preferredHeight: fileBrowser.height
            }
        }

        RowLayout {
            ColumnLayout {
                Rectangle {
                    Layout.fillWidth: true
                    // Layout.fillHeight: true
                    Layout.preferredHeight: 50
                    Layout.minimumHeight: 30

                    color: "transparent"
                }

                DownloadStatusPanel {}
            }
            Image {
                id: kitties

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.margins: 20

                source: "qrc:Images/kitties.png"

                fillMode: Image.PreserveAspectFit
            }
        }
    }

    DownloadMenu {
        id: downloadMenu
    }
}
