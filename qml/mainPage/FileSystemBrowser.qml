import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property string currentPath: "/"
    property string currentName: ""
    property int currentSize: 0
    property int fileIndex: -1

    Component.onCompleted: {
        Client.getSystemDirectory(currentPath)
    }

    Layout.fillWidth: true
    Layout.minimumWidth: 100
    Layout.preferredHeight: Window.height * 0.6
    Layout.minimumHeight: 250

    radius: 4
    color: "white"

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

            font.pointSize: 12
            color: "black"
            text: "File System"
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
                font.pixelSize: 14
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
                font.pixelSize: 14
                renderType: Text.NativeRendering
            }
            Text {
                anchors.left: parent.left
                anchors.margins: 20
                text: ".."
                font.pixelSize: 14
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
                    Client.getSystemDirectory(root.currentPath)
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

            model: SystemDirectoryManager

            delegate: Rectangle {
                id: click
                width: root.width - 10
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
                    onClicked: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
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
                        } else if (mouse.button === Qt.RightButton) {
                            isRightClicked = true
                        }
                    }
                    onDoubleClicked: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            console.log("File path:", model.path)

                            listView.currentIndex = index
                            listView.forceActiveFocus()

                            if (model.isDir && model.isReadable) {
                                root.currentPath = model.path
                                Client.getSystemDirectory(root.currentPath)
                                fileIndex = -1
                            }
                        }
                    }

                    CustomToolTip {
                        id: tooltip

                        visibilityController: mouse
                        visibilityMode: mouse.containsMouse
                                        && !mouse.isRightClicked

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

                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    property bool isRightClicked: false

                    CustomToolTip {
                        id: tooltipRi

                        visibilityController: mouse
                        visibilityMode: mouse.isRightClicked

                        contentItem: Rectangle {
                            color: "red"
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

            // Keys.onUpPressed: {
            //     if (listView.currentIndex === 0 && backToDir.visible) {
            //         listView.currentIndex = -1
            //         backToDir.forceActiveFocus()
            //     }
            // }
            // Keys.onDownPressed: {
            //     if (listView.model.count > 0) {
            //         listView.currentIndex = 0
            //         listView.forceActiveFocus()
            //     }
            // }
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

            // CustomButton {
            //     id: connectionButton

            //     buttonText: "Connect"
            //     onClicked: {
            //         Client.connectToServer()
            //     }
            // }
            Item {
                Layout.fillWidth: true
            }
        }
    }
}
