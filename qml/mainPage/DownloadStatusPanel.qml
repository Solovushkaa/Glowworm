import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    Layout.preferredWidth: contentArea.width
    Layout.minimumWidth: 100
    Layout.fillHeight: true
    Layout.minimumHeight: 15
    Layout.bottomMargin: 20

    // function finishDownload(downloadID) {
    //     // some code
    // }
    // Connections {
    //     target: Client

    //     function onFileSuccessfulReceived(downloadID) {
    //         finishDownload(downloadID)
    //     }
    // }
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
            width: contentArea.width - 10
            height: 22
            focus: true

            color: ListView.isCurrentItem ? (downloadStatusList.activeFocus ? "lightgrey" : "#e9e9e9") : "white"

            Rectangle {
                id: playPauseBound

                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                visible: (model.downloadState !== 3)
                width: parent.width * 0.03

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
                        margins: 5
                    }
                    source: model.downloadState === 2 ? "qrc:Icons/pause.svg" : "qrc:Icons/play.svg"
                    sourceSize.width: 100
                    sourceSize.height: 100

                    // smooth: true
                    antialiasing: true
                    mipmap: true

                    fillMode: Image.PreserveAspectFit
                }
                ColorOverlay {
                    anchors.fill: playPause
                    source: playPause
                    color: "#5371ad"
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
                font.pointSize: 11
                renderTypeQuality: Text.NormalRenderTypeQuality
            }

            Text {
                id: progressBarTextBlack
                // anchors.centerIn: parent
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                // textFormat: Text.RichText
                text: Math.round(
                          (model.lastReceivedByte
                           === 0) ? 0 : (model.lastReceivedByte / model.size) * 1000) / 10 + "%"

                font.pointSize: 11
                renderTypeQuality: Text.NormalRenderTypeQuality

                color: "black"
                visible: true
            }

            CustomProgressBar {
                id: progressBar

                anchors {
                    top: parent.top
                    topMargin: parent.height * 0.1
                    right: progressBarTextBlack.left
                    rightMargin: 5
                    bottom: parent.bottom
                    bottomMargin: parent.height * 0.1
                }
                value: (model.lastReceivedByte === 0) ? 0 : (model.lastReceivedByte / model.size)

                width: parent.width * 0.15
            }

            Text {
                id: progressBarTextBytes
                // anchors.centerIn: parent
                anchors {
                    right: progressBar.left
                    rightMargin: 120
                    verticalCenter: parent.verticalCenter
                }

                text: Math.round(
                          model.lastReceivedByte / (1024 * 1024) * 10) / 10 + "/"
                      + Math.round(model.size / (1024 * 1024) * 10) / 10 + " MB"

                font.pointSize: 11
                renderTypeQuality: Text.NormalRenderTypeQuality

                color: "black"
                visible: true
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
                    updateDownloadInfoPanel(model.name, "file", model.path,
                                            model.size, model.lastReceivedByte,
                                            model.downloadState)
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
