import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import CustomButtons
import SavedConnectionManager
import HttpClient

Rectangle {
    id: root
    anchors {
        top: parent.top
        left: parent.left
        bottom: parent.bottom
    }

    property int m_activeConnection: 0
    property bool isOpen: false
    property bool isAnimated: true
    property int animationInterval: 150

    function loadPresetsToList() {
        presetsModel.clear()

        var presets = ConnectionManager.getConnections()

        for (var i = 0; i < presets.length; i++) {
            presetsModel.append(presets[i])
        }

        loadActivePreset()
    }

    Connections {
        target: ConnectionManager
        function onConnectionsLoaded() {
            if (ConnectionManager.isEmpty()) {
                startAddPopup.open()
            } else {
                loadPresetsToList()
            }
        }
        function onConnectionsChanged() {
            isPageInteractiveActive = false
        }
    }

    width: isOpen ? Math.min(Math.max(window.width * 0.2, 50), 400) : 50
    z: 300

    color: "#efefef"

    Rectangle {

        visible: isOpen

        anchors {
            top: parent.top
            left: parent.right
            bottom: parent.bottom
        }

        width: 2
        color: "#efefef"
    }

    BurgerButton {
        id: burgerButton
        onClicked: {
            isAnimated = true
            isOpen = !isOpen
            borderWidth = isOpen ? 0 : 1

            isAnimated = false
        }
    }

    Behavior on width {
        enabled: isAnimated
        NumberAnimation {
            duration: animationInterval
        }
    }

    ListView {
        id: savedPresets

        spacing: 0

        anchors {
            top: burgerButton.bottom
            left: parent.left
            right: parent.right
            bottom: settingsGear.top
            leftMargin: 5
            rightMargin: 5
            topMargin: 10
            bottomMargin: 10
        }

        clip: true

        currentIndex: -1

        boundsBehavior: Flickable.StopAtBounds

        visible: isOpen

        model: ListModel {
            id: presetsModel
        }

        delegate: Rectangle {

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 40

            color: ListView.isCurrentItem ? "#cfcfcf" : "transparent"

            radius: 4

            Rectangle {
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                    topMargin: -1
                }
                width: parent.width * 0.75
                height: 1
                opacity: 0.3
                color: "#3d5482"
            }

            Rectangle {
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }

                width: 3
                color: "lightgreen"

                visible: (m_activeConnection === index)

                radius: 2
            }

            Text {
                anchors {
                    fill: parent
                    leftMargin: 7
                    rightMargin: 7
                }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                color: "black"

                text: name
                clip: true

                elide: Text.ElideRight

                font.pointSize: 11
            }

            Rectangle {
                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                width: parent.width * 0.75
                height: 1
                opacity: 0.3
                color: "#3d5482"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    savedPresets.currentIndex = index
                }
                onDoubleClicked: {
                    savedPresets.currentIndex = index

                    ConnectionManager.setActive(index)
                    m_activeConnection = index
                    fileModel.clear()
                    mainPageHeader.m_activeConnection = ConnectionManager.getActive(
                                )["name"].toString()
                    Client.checkConnectionToServer()
                }
            }

            CustomImage {
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: 4
                }
                width: 15
                height: 15

                source: "qrc:/Content/Icons/delete.svg"

                MouseArea {
                    anchors.centerIn: parent.centerIn
                    width: parent.width + 4
                    height: parent.height + 4
                }
            }
        }

        footer: Rectangle {
            width: 30
            height: 40

            color: "transparent"

            Image {
                id: plus

                anchors {
                    fill: parent
                    topMargin: 10
                }

                source: "qrc:/Content/Icons/plus.svg"

                visible: false

                smooth: true
                antialiasing: true
                mipmap: true

                fillMode: Image.PreserveAspectFit
            }
            ColorOverlay {
                anchors.fill: plus
                source: plus
                color: "#3d5482"

                opacity: 0.6
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    addPopup.open()
                }
            }
        }
    }

    Rectangle {
        id: settingsGear

        visible: isOpen

        anchors {
            left: parent.left
            bottom: parent.bottom
            right: parent.right
        }

        height: 60

        color: "transparent"

        Button {
            anchors {
                fill: parent
                topMargin: 5
                leftMargin: 5
                bottomMargin: 5

                rightMargin: parent.width * 0.8
            }

            background: Rectangle {
                anchors.fill: parent
                color: "transparent"
            }

            Image {
                id: settings

                anchors.fill: parent

                source: "qrc:/Content/Icons/settings.svg"

                visible: false

                smooth: true
                antialiasing: true
                mipmap: true

                fillMode: Image.PreserveAspectFit
            }
            ColorOverlay {
                anchors.fill: settings
                source: settings
                color: "#3d5482"
            }

            onClicked: {
                pagesStack.push(settingsPage)
            }
        }
    }

    Rectangle {
        visible: isOpen
        z: 200
        anchors {
            top: root.top
            left: root.right
            bottom: root.bottom
        }
        width: window.width - root.width
        color: "#000"
        opacity: isOpen ? 0.2 : 0

        MouseArea {
            anchors.fill: parent
            onClicked: {
                burgerButton.click()
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }
    }
}
