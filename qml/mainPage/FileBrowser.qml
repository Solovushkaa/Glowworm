import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CustomButtons

Rectangle {
    id: root

    property string currentPath: "/"
    property string currentName: ""
    property int currentSize: 0
    property int fileIndex: -1

    property bool isConnect: true

    Layout.fillWidth: true
    Layout.minimumWidth: 100
    Layout.preferredHeight: Window.height * 0.6
    Layout.minimumHeight: 250

    radius: 4
    color: "white"

    property string activeConnectionName: ""

    Connections {
        target: ClientConnectionManager
        function onActiveConnectionChanged() {
            activeConnectionName = ClientConnectionManager.getActiveConnection(
                        ) !== null ? ClientConnectionManager.getActiveConnection(
                                         ).name : ""
        }
        function onConnected() {
            isConnect = false
            Client.getNetworkDirectory(currentPath)
        }
    }

    function updateFileInfoPanel(fileInfo, name, type, location, size, created, modified, accessed) {
        fileInfo.name = name
        fileInfo.typeName = type
        fileInfo.locationName = location
        fileInfo.sizeName = size
        fileInfo.createdName = created
        fileInfo.modifiedName = modified
        fileInfo.accessedName = accessed
    }

    Rectangle {
        color: "transparent"
        anchors {
            left: parent.left
            leftMargin: 10
            bottom: parent.top
            bottomMargin: 15
        }

        width: 300

        Label {
            anchors {
                verticalCenter: parent.verticalCenter
            }

            font.pointSize: 13
            renderTypeQuality: Text.NormalRenderTypeQuality
            color: "black"
            text: activeConnectionName
        }
    }

    Rectangle {
        implicitWidth: 100
        height: mainPageHeader.height + 4

        anchors {
            right: root.right
            rightMargin: 2
            bottom: root.top
            bottomMargin: 4
        }
        color: "transparent"

        Text {
            text: "Server:"
            color: "#505050"

            font.pointSize: 11
            renderTypeQuality: Text.HighRenderTypeQuality

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
                    font.pointSize: 10
                    renderTypeQuality: Text.NormalRenderTypeQuality
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
                    font.pointSize: 10
                    renderTypeQuality: Text.NormalRenderTypeQuality
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
                renderTypeQuality: Text.NormalRenderTypeQuality
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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5
        spacing: 0

        Rectangle {

            Layout.preferredHeight: 20
            Layout.fillWidth: true

            Text {
                text: root.currentPath
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 11
                renderTypeQuality: Text.NormalRenderTypeQuality
            }
        }

        Item {
            Layout.preferredHeight: 2
        }

        Rectangle {
            id: backToDir
            Layout.preferredHeight: 20
            Layout.fillWidth: true
            visible: root.currentPath !== "/"

            color: listView.currentIndex
                   === -1 ? (listView.activeFocus ? "lightgrey" : "#e9e9e9") : "white"

            Text {
                anchors.left: parent.left
                text: "📁"
                font.pointSize: 11
                renderTypeQuality: Text.NormalRenderTypeQuality
                renderType: Text.NativeRendering
            }
            Text {
                anchors.left: parent.left
                anchors.margins: 20
                text: ".."
                font.pointSize: 11
                renderTypeQuality: Text.NormalRenderTypeQuality
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    listView.currentIndex = -1
                    listView.forceActiveFocus()
                    // Обновляем информационную панель (аналогично делегату)
                    currentName = ".."
                    currentSize = ""
                    fileIndex = -1
                    updateFileInfoPanel(fileInfo, "..", "📁",
                                        root.currentPath.slice(
                                            0, root.currentPath.lastIndexOf(
                                                "/")) || "/", "", "", "", "")
                }
                onDoubleClicked: {
                    root.currentPath = root.currentPath.slice(
                                0, root.currentPath.lastIndexOf("/")) || "/"
                    Client.getNetworkDirectory(root.currentPath)
                    fileIndex = -1
                    listView.currentIndex = -1
                }
            }
        }
        ListView {
            id: listView
            Layout.fillHeight: true
            Layout.fillWidth: true

            highlightMoveDuration: 0
            highlightResizeDuration: 0

            focus: true
            clip: true

            boundsBehavior: Flickable.StopAtBounds

            currentIndex: -1

            model: NetworkDirectoryManager

            delegate: Rectangle {
                id: click
                width: root.width - 10
                height: 20
                focus: true
                color: ListView.isCurrentItem ? (listView.activeFocus ? "lightgrey" : "#e9e9e9") : "white"

                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.isReadable ? (model.isDir ? "📁" : "📄") : "🔒"
                    font.pointSize: 11
                    renderTypeQuality: Text.NormalRenderTypeQuality
                    renderType: Text.NativeRendering
                }

                Text {
                    anchors.left: parent.left
                    anchors.margins: 20
                    text: model.name
                    font.pointSize: 11
                    renderTypeQuality: Text.NormalRenderTypeQuality
                }

                Text {
                    anchors.right: parent.right
                    text: model.isDir ? "" : (Math.round(
                                                  (model.size / (1024 * 1024))
                                                  * 100) / 100).toString(
                                            ) + "MB"
                    font.pointSize: 11
                    renderTypeQuality: Text.NormalRenderTypeQuality
                }

                MouseArea {
                    id: mouse

                    anchors.fill: parent

                    hoverEnabled: true
                    onHoveredChanged: {
                        console.log("Hovered element:", listView.currentIndex)

                        currentName = model.name
                        currentSize = model.size

                        updateFileInfoPanel(fileInfo, model.name,
                                            model.isDir ? "📁" : "📄",
                                            model.path,
                                            model.isDir ? "" : model.size,
                                            model.created, model.modified,
                                            model.accessed)
                    }
                    onClicked: {
                        listView.currentIndex = index

                        console.log("Element selected:", model.name)

                        currentName = model.name
                        currentSize = model.size

                        fileIndex = listView.currentIndex
                        listView.forceActiveFocus()

                        updateFileInfoPanel(fileInfo, model.name,
                                            model.isDir ? "📁" : "📄",
                                            model.path,
                                            model.isDir ? "" : model.size,
                                            model.created, model.modified,
                                            model.accessed)
                    }
                    onDoubleClicked: {
                        console.log("File path:", model.path)

                        listView.currentIndex = index
                        listView.forceActiveFocus()

                        if (model.isDir && model.isReadable) {
                            currentPath = model.path
                            Client.getNetworkDirectory(currentPath)
                            fileIndex = -1
                        }
                    }

                    CustomToolTip {
                        id: tooltip

                        visibilityController: mouse

                        contentItem: FileInfoPanel {
                            id: fileInfo
                            width: Screen.width * 0.08 - 2
                            height: Screen.height * 0.15 - 2
                            anchors.centerIn: parent
                            opacity: tooltip.visible ? 1 : 0
                            Behavior on opacity {
                                NumberAnimation {
                                    duration: 200
                                    easing.type: Easing.OutCubic
                                }
                            }
                        }
                    }
                }
            }
            ScrollBar.vertical: ScrollBar {
                id: verticalScrollBar

                parent: listView.parent.parent

                anchors {
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                }
            }
        }
    }

    Rectangle {
        anchors {
            top: root.bottom
            left: root.left
            right: root.right
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
                id: connectDisconnectButton

                buttonText: root.isConnect ? "Connect" : "Disconnect"
                onClicked: {
                    // if (root.isConnect) {
                    //     Client.connectToServer()
                    // } else {
                    //     Client.disconnectFromServer()
                    // }
                    Client.getNetworkDirectory(currentPath)
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

    DownloadMenu {
        id: downloadMenu
    }
}
