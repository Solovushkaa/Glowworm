import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: mainPage
    anchors.fill: parent

    background: Rectangle {
        color: "#eeeeee"
    }

    property ApplicationWindow mainWindow: null

    property bool isPageInteractiveActive: true // (ClientConnectionManager.getActiveConnection().connectionState === 1)
    // property var fileBrows: fileBrowser

    // Connections {
    //     target: Client
    //     function onConnectionStatusCodeChanged(currentStatusCode) {
    //         if (currentStatusCode === 200)
    //             mainPage.isPageInteractiveActive = true
    //         else
    //             mainPage.isPageInteractiveActive = false
    //     }
    // }
    // function updateFileInfoPanel(name, type, location, size, created, modified, accessed) {
    //     fileInfo.name = name
    //     fileInfo.typeName = type
    //     fileInfo.locationName = location
    //     fileInfo.sizeName = size
    //     fileInfo.createdName = created
    //     fileInfo.modifiedName = modified
    //     fileInfo.accessedName = accessed
    // }

    // function updateDownloadInfoPanel(name, type, path, size, lastReceivedByte, downloadState) {
    //     downloadInfo.downloadName = name
    //     downloadInfo.downloadType = type
    //     downloadInfo.downloadLocation = path
    //     downloadInfo.downloadSize = size
    //     downloadInfo.downloadLastReceivedByte = lastReceivedByte
    //     downloadInfo.downloadState = downloadState
    // }
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

        width: 60
    }
    Item {
        id: mirrorBurgerCollision
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }

        width: 60
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
            right: mirrorBurgerCollision.left
            bottom: parent.bottom
        }

        RowLayout {
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.topMargin: 40

            FileSystemBrowser {
                id: fileSystem
            }
            FileBrowser {
                id: remoteDevice
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
            // Rectangle {
            //     id: downloadInfo

            //     Layout.fillHeight: true
            //     Layout.fillWidth: true
            //     Layout.margins: 20

            //     color: "#ffffff"
            //     radius: 4

            //     property int elementHeight: downloadInfo.height / 3

            //     property string downloadName: ""
            //     property string downloadType: ""
            //     property string downloadLocation: ""
            //     property string downloadSize: ""
            //     property string downloadLastReceivedByte: ""
            //     property int downloadState: 0

            //     ColumnLayout {
            //         anchors.fill: parent

            //         spacing: 0

            //         Area {
            //             id: downloadNameID
            //             isDelimiterVisible: false
            //             prefix: "Name: "
            //             name: downloadInfo.downloadName
            //             elementHeight: downloadInfo.elementHeight
            //         }
            //         // Area {
            //         //     id: downloadLocationID
            //         //     prefix: "Location: "
            //         //     name: downloadLocation
            //         // }
            //         Area {
            //             id: downloadSizeID
            //             prefix: "Size: "
            //             name: downloadInfo.downloadSize
            //             postfix: downloadInfo.downloadName
            //                      === "" ? (downloadInfo.downloadType
            //                                === "Directory" ? "-" : "") : "B"
            //             elementHeight: downloadInfo.elementHeight
            //         }
            //         Area {
            //             id: downloadLastReceivedByteID
            //             prefix: "Last received byte: "
            //             name: downloadInfo.downloadLastReceivedByte
            //             postfix: downloadInfo.downloadName
            //                      === "" ? (downloadInfo.downloadType
            //                                === "Directory" ? "-" : "") : "B"
            //             elementHeight: downloadInfo.elementHeight
            //         }
            //         // Area {
            //         //     id: downloadStateID
            //         //     prefix: "Last received byte: "
            //         //     name: downloadState === 3 ? "Active" : "Pause"
            //         // }
            //     }
            // }
        }
    }

    DownloadMenu {
        id: downloadMenu
    }
}
