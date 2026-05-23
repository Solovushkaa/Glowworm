import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    Layout.preferredWidth: fileBrowser.width
    Layout.minimumWidth: 100
    Layout.fillHeight: true
    Layout.minimumHeight: 15
    Layout.bottomMargin: 20

    // function finishDownload(downloadID) {
    //     // some code
    // }
    Connections {
        target: Client

        // function onFileSuccessfulReceived(downloadID) {
        //     finishDownload(downloadID)
        // }
    }

    radius: 8

    ListView {
        id: downloadStatusList
        anchors.fill: parent
        anchors.margins: 5

        highlightMoveDuration: 0
        highlightResizeDuration: 0

        focus: true

        clip: true

        boundsBehavior: Flickable.StopAtBounds

        currentIndex: -1

        model: DownloadManager

        delegate: Rectangle {
            id: click
            width: fileBrowser.width - 10
            height: 20
            focus: true
            color: "white"

            Rectangle {
                // TODO: visible only if not finished
                id: playPauseBound

                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                visible: (model.downloadState !== 4)
                width: parent.width * 0.05

                color: "transparent"

                Behavior on width {
                    enabled: true
                    NumberAnimation {
                        duration: 75
                    }
                }

                Image {
                    id: playPause
                    anchors {
                        fill: parent
                        topMargin: 3
                        bottomMargin: 3
                    }
                    source: model.downloadState === 2 ? "qrc:Icons/play.svg" : "qrc:Icons/pause.svg"
                    fillMode: Image.PreserveAspectFit

                    smooth: true
                    antialiasing: true
                    mipmap: true
                }
                ColorOverlay {
                    anchors.fill: playPause
                    source: playPause
                    color: "#5371ad"

                    opacity: 0.6
                }

                MouseArea {
                    anchors.fill: parent
                    visible: !model.isBlockedPlayPause
                    onClicked: {
                        if (model.downloadState === 2) {
                            model.downloadState = 0
                            Client.stopDownload(model.downloadID)

                            console.log("Download stopped")
                        } else if (model.downloadState === 0) {
                            model.downloadState = 2
                            Client.startDownload(model.downloadID)

                            console.log("Download continued")
                        }
                    }
                }
            }

            Text {
                anchors {
                    left: playPauseBound.right
                    verticalCenter: parent.verticalCenter
                }
                text: model.name
                font.pixelSize: 14
            }

            CustomProgressBar {
                anchors {
                    top: parent.top
                    topMargin: parent.height * 0.12
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: parent.height * 0.12
                }
                value: (model.lastReceivedByte === 0) ? 0 : (model.lastReceivedByte / model.size)

                width: parent.width * 0.15
            }

            MouseArea {
                anchors {
                    left: playPauseBound.right
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                }
                onClicked: {
                    downloadStatusList.currentIndex = index
                    downloadStatusList.forceActiveFocus()
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            anchors.top: downloadStatusList.top
            anchors.left: downloadStatusList.right
            anchors.bottom: downloadStatusList.bottom
            anchors.leftMargin: 2
        }
    }
}
