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
    property bool isPageInteractiveActive: true // (ClientConnectionManager.getActiveConnection().connectionState === 1)
    property var fileBrows: fileBrowser

    // Connections {
    //     target: Client
    //     function onConnectionStatusCodeChanged(currentStatusCode) {
    //         if (currentStatusCode === 200)
    //             mainPage.isPageInteractiveActive = true
    //         else
    //             mainPage.isPageInteractiveActive = false
    //     }
    // }
    function updateFileInfoPanel(name, type, location, size, created, modified, accessed) {
        fileInfo.name = name
        fileInfo.typeName = type
        fileInfo.locationName = location
        fileInfo.sizeName = size
        fileInfo.createdName = created
        fileInfo.modifiedName = modified
        fileInfo.accessedName = accessed
    }

    function updateDownloadInfoPanel(name, type, path, size, lastReceivedByte, downloadState) {
        downloadInfo.downloadName = name
        downloadInfo.downloadType = type
        downloadInfo.downloadLocation = path
        downloadInfo.downloadSize = size
        downloadInfo.downloadLastReceivedByte = lastReceivedByte
        downloadInfo.downloadState = downloadState
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

    // footer: Footer {
    //     id: mainFooter
    // }
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
            id: mainRowLayout
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

                Rectangle {
                    implicitWidth: 100
                    height: mainPageHeader.height + 4

                    anchors {
                        right: fileBrowser.right
                        rightMargin: 2
                        bottom: fileBrowser.top
                        bottomMargin: 4
                    }
                    color: "transparent"

                    Text {
                        text: "Server:"
                        color: "#505050"

                        font.pointSize: 12

                        anchors {
                            horizontalCenter: parent.left
                            verticalCenter: parent.verticalCenter
                        }
                    }
                    Switch {
                        id: control
                        implicitWidth: 60
                        height: mainPageHeader.height + 4

                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                        }

                        checked: false

                        indicator: Rectangle {
                            width: parent.width
                            height: parent.height
                            radius: 20

                            // color: control.checked ? "#4caf50" : "#e0e0e0"
                            color: control.checked ? "#5371ad" : "#e0e0e0"

                            Behavior on color {
                                ColorAnimation {
                                    duration: 200
                                    easing.type: Easing.InOutQuad
                                }
                            }

                            Text {
                                visible: control.checked

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
                                visible: !control.checked

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

                                x: control.checked ? parent.width - width - 2 : 2

                                Behavior on x {
                                    NumberAnimation {
                                        duration: 200
                                        easing.type: Easing.OutCubic
                                    }
                                }
                            }
                        }

                        contentItem: Text {
                            text: control.text
                            font.pointSize: 11
                            leftPadding: control.indicator.width + 8
                            verticalAlignment: Text.AlignVCenter
                            visible: control.text !== ""
                        }

                        onClicked: {
                            if (checked) {
                                Server.startServers()
                            } else {
                                Server.stopServers()
                            }
                        }
                    }
                }

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

                            // visible: ClientConnectionManager.hasActiveConnection
                            //          && !isPageInteractiveActive
                            // isActive: (ClientConnectionManager.hasActiveConnection
                            //            && ClientConnectionManager.getActiveConnection(
                            //                ).connectionState !== ConnectionInfo.Connected)
                            buttonText: "Connect"
                            onClicked: {
                                Client.connectToServer()
                            }
                        }
                        CustomButton {
                            id: disconnectButton

                            // visible: ClientConnectionManager.hasActiveConnection
                            //          && isPageInteractiveActive
                            // isActive: ClientConnectionManager.hasActiveConnection
                            buttonText: "Disconnect"
                            onClicked: {
                                Client.disconnectFromServer()
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
                                    downloadMenu.headerText = currentName
                                    downloadMenu.open()
                                } else {
                                    console.log("Download error")
                                }
                            }
                        }
                        CustomButton {
                            id: generateQuickConnectKeyButton

                            buttonText: "Share key"
                            onClicked: {
                                keyGenPopup.open()
                            }
                        }
                        // CustomButton {
                        //     id: connectToRelayServer

                        //     buttonText: "Connect relay server"
                        //     onClicked: {
                        //         Client.connectToRelayServer()
                        //     }
                        // }
                        // CustomButton {
                        //     id: downloadFromRelayServer

                        //     buttonText: "Dw Rserv"
                        //     onClicked: {

                        //         Client.getFileFromRelayServer()
                        //     }
                        // }
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
                    Layout.preferredWidth: Window.width * 0.7
                    Layout.minimumWidth: 100
                    // Layout.fillHeight: true
                    Layout.preferredHeight: 50
                    Layout.minimumHeight: 30

                    color: "transparent"
                }

                DownloadStatusPanel {}
            }

            // Image {
            //     id: kitties

            //     Layout.fillHeight: true
            //     Layout.fillWidth: true
            //     Layout.margins: 20

            //     source: "qrc:Images/kitties.png"

            //     fillMode: Image.PreserveAspectFit
            // }
            Rectangle {
                id: downloadInfo

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.margins: 20

                color: "#ffffff"
                radius: 4

                property int elementHeight: downloadInfo.height / 3

                property string downloadName: ""
                property string downloadType: ""
                property string downloadLocation: ""
                property string downloadSize: ""
                property string downloadLastReceivedByte: ""
                property int downloadState: 0

                ColumnLayout {
                    anchors.fill: parent

                    spacing: 0

                    Area {
                        id: downloadNameID
                        isDelimiterVisible: false
                        prefix: "Name: "
                        name: downloadInfo.downloadName
                        elementHeight: downloadInfo.elementHeight
                    }
                    // Area {
                    //     id: downloadLocationID
                    //     prefix: "Location: "
                    //     name: downloadLocation
                    // }
                    Area {
                        id: downloadSizeID
                        prefix: "Size: "
                        name: downloadInfo.downloadSize
                        postfix: downloadInfo.downloadName
                                 === "" ? (downloadInfo.downloadType
                                           === "Directory" ? "-" : "") : "B"
                        elementHeight: downloadInfo.elementHeight
                    }
                    Area {
                        id: downloadLastReceivedByteID
                        prefix: "Last received byte: "
                        name: downloadInfo.downloadLastReceivedByte
                        postfix: downloadInfo.downloadName
                                 === "" ? (downloadInfo.downloadType
                                           === "Directory" ? "-" : "") : "B"
                        elementHeight: downloadInfo.elementHeight
                    }
                    // Area {
                    //     id: downloadStateID
                    //     prefix: "Last received byte: "
                    //     name: downloadState === 3 ? "Active" : "Pause"
                    // }
                }
            }
        }
    }

    DownloadMenu {
        id: downloadMenu
    }
}
