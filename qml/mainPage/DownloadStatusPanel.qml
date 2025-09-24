import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import HttpClient
import SavedConnectionManager

Rectangle {
    id: root

    Layout.preferredWidth: fileBrowser.width
    Layout.minimumWidth: 100
    Layout.fillHeight: true
    Layout.minimumHeight: 15
    Layout.bottomMargin: 20

    function openUnfinishedDownloads() {
        var unfinishedDownloads = Client.getUnfinishedDownloads()

        if (unfinishedDownloads.length === 0) {
            console.log(unfinishedDownloads.length)
        } else {
            for (var i = 0; i < unfinishedDownloads.length; i++) {
                fileModelDownloadStatus.append({
                                                   "fileName": unfinishedDownloads[i]["fileName"],
                                                   "downloadID": unfinishedDownloads[i]["downloadID"],
                                                   "fileSize": parseInt(
                                                                   unfinishedDownloads[i]["fileSize"]),
                                                   "fileLastReceivedByte": parseInt(unfinishedDownloads[i]["fileLastReceivedByte"]),
                                                   "isPlayPauseButtonActive": true,
                                                   "isBlockedPlayPause": false
                                               })
            }
        }
    }

    function finishDownload(downloadID) {
        console.log("Finish Download")
        for (var i = 0; i < fileModelDownloadStatus.count; i++) {
            if (fileModelDownloadStatus.get(i).downloadID === downloadID) {
                fileModelDownloadStatus.get(i).isPlayPauseButtonActive = false
                fileModelDownloadStatus.get(
                            i).fileLastReceivedByte = fileModelDownloadStatus.get(
                            i).fileSize
                break
            }
        }
    }

    Component.onCompleted: {
        openUnfinishedDownloads()
    }

    Connections {
        target: Client
        function onNewDownload(fileName, downloadID, fileSize) {
            fileModelDownloadStatus.append({
                                               "fileName": fileName,
                                               "downloadID": downloadID,
                                               "fileSize": fileSize,
                                               "fileLastReceivedByte": 0,
                                               "isPlayPauseButtonActive": true,
                                               "isBlockedPlayPause": false
                                           })
        }

        function onChangeProgress(downloadID, bytesReceived) {
            for (var i = 0; i < fileModelDownloadStatus.count; i++) {
                if (fileModelDownloadStatus.get(i).downloadID === downloadID) {
                    fileModelDownloadStatus.get(
                                i).fileLastReceivedByte = bytesReceived
                    fileModelDownloadStatus.get(i).isBlockedPlayPause = false
                    break
                }
            }
        }

        function onStopProgress(downloadID, bytesReceived) {
            for (var i = 0; i < fileModelDownloadStatus.count; i++) {
                if (fileModelDownloadStatus.get(i).downloadID === downloadID) {
                    fileModelDownloadStatus.get(
                                i).fileLastReceivedByte = bytesReceived
                    fileModelDownloadStatus.get(i).isBlockedPlayPause = false
                    break
                }
            }
        }

        function onRangeRequestSuccessfulFinished(downloadID) {
            finishDownload(downloadID)
        }
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

        model: ListModel {
            id: fileModelDownloadStatus
        }

        delegate: Rectangle {
            id: click
            width: fileBrowser.width - 10
            height: 20
            focus: true
            color: "white"

            property bool isDownloadActive: false

            Rectangle {
                id: playPauseBound

                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: model.isPlayPauseButtonActive ? parent.width * 0.05 : parent.width * 0.01
                visible: model.isPlayPauseButtonActive

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
                    source: isDownloadActive ? "qrc:/Content/Icons/pause.svg" : "qrc:/Content/Icons/play.svg"

                    fillMode: Image.PreserveAspectFit

                    smooth: true
                    antialiasing: true
                    mipmap: true
                }
                ColorOverlay {
                    anchors.fill: playPause
                    source: playPause
                    color: isDownloadActive ? "#cc1616" : "#14cc1b"

                    opacity: 0.6
                }

                MouseArea {
                    anchors.fill: parent
                    visible: !model.isBlockedPlayPause
                    onClicked: {
                        isDownloadActive = !isDownloadActive
                        model.isBlockedPlayPause = true

                        if (!isDownloadActive) {
                            console.log("Скачивание продолжено")
                            Client.startDownload(model.downloadID)
                        } else {
                            console.log("Скачивание остановлено")
                            Client.stopDownload(model.downloadID)
                        }
                    }
                }
            }

            Text {
                anchors {
                    left: playPauseBound.right
                    verticalCenter: parent.verticalCenter
                }
                text: fileName
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
                value: (model.fileLastReceivedByte
                        === 0) ? 0 : (model.fileLastReceivedByte / model.fileSize)

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
