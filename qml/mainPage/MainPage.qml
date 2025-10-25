import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import HttpClient
import CustomButtons
import ClientSavedConnectionManager

Page {
    id: mainPage
    anchors.fill: parent

    background: Rectangle {
        color: "#eeeeee"
    }

    property string currentPath: "/"
    property string currentName: ""
    property int currentSize: 0
    property string downloadPath: ""
    property bool isPageInteractiveActive: false

    Connections {
        target: Client
        function onRequestSuccessfulFinished() {
            if (typeof updateDirectoryLayout === "function")
                updateDirectoryLayout()
        }
        function onChangeStatusCode(currentStatusCode) {
            // if (mainFooter)
            //     mainFooter.footerText = "Current connection code: " + currentStatusCode + "\n"
            if (currentStatusCode === 200)
                isPageInteractiveActive = true
        }
        function onRangeRequestSuccessfulFinished() {
            if (mainFooter)
                mainFooter.footerText = "Range request successful ended\n"
        }
    }

    function updateDirectoryLayout() {
        var files = Client.getCurrentDirectory()

        mainFooter.footerText = "Directory successful get!\n"

        for (var i = 0; i < files.length; i++) {
            if (files[i]["path"] === "/..") {

            } else {
                fileModel.append(files[i])
            }
        }
    }
    function getDirectoryLayout() {
        fileModel.clear()
        console.log("Запрос на ресурс")

        Client.getDirectoryList(currentPath)
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
    function loadActivePreset() {
        console.log("Active")

        mainPageHeader.m_activeConnection = ConnectionManager.getActive(
                    )["name"].toString()

        Client.startConnectionVerification()

        isPageInteractiveActive = false

        mainFooter.footerText = "Preset Changed!"
    }

    BurgerMenu {
        id: burgerMenu
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

                    model: ListModel {
                        id: fileModel
                    }

                    delegate: Rectangle {
                        id: click
                        width: fileBrowser.width - 10
                        height: 20
                        focus: true
                        color: ListView.isCurrentItem ? (listView.activeFocus ? "lightgrey" : "#e9e9e9") : "white"

                        Text {
                            anchors.left: parent.left
                            text: isReadable ? (isDir ? "📁" : "📄") : "🔒"
                            font.pixelSize: 14
                        }

                        Text {
                            anchors.left: parent.left
                            anchors.margins: 20
                            text: name
                            font.pixelSize: 14
                        }

                        Text {
                            anchors.right: parent.right
                            text: isDir ? "" : size
                            font.pixelSize: 14
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                listView.currentIndex = index
                                console.log("Выбран элемент:", model.name)
                                currentName = model.name
                                currentSize = model.size
                                downloadPath = model.path
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
                                    getDirectoryLayout()
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
                            id: getDirectoryButton
                            isActive: isPageInteractiveActive

                            buttonText: "Connect"
                            onClicked: {
                                getDirectoryLayout()
                            }
                        }
                        CustomButton {
                            id: downloadContentButton
                            isActive: isPageInteractiveActive

                            buttonText: "Download"
                            onClicked: {
                                if (downloadPath != "") {
                                    var part = fileModel.get(
                                                listView.currentIndex).isDir ? " Full directory " : " file "
                                    downloadMenu.headerText = "Download" + part
                                            + "\"" + currentName + "\"?"
                                    console.log(part)
                                    downloadMenu.open()
                                }
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

                source: "qrc:/Content/Images/kitties.jpeg"

                fillMode: Image.PreserveAspectFit
            }
        }
    }

    DownloadMenu {
        id: downloadMenu
    }
}
