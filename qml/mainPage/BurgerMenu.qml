import QtQuick
import QtQuick.Controls
// import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import CustomButtons

Rectangle {
    id: root
    anchors {
        top: parent.top
        left: parent.left
        bottom: parent.bottom
    }

    property ApplicationWindow mainWindow: null

    property bool isOpen: false
    property bool isAnimated: true
    property int animationInterval: 250

    property int activeConnectionIndex: -1

    // Connections {
    //     target: ClientConnectionManager
    //     function onConnectionsLoaded() {
    //         if (ConnectionManager.isEmpty()) {
    //             startAddPopup.open()
    //         } else {
    //             loadPresetsToList()
    //         }
    //     }
    //     function onConnectionsChanged() {
    //         isPageInteractiveActive = false
    //     }
    // }
    width: root.isOpen ? Math.min(Math.max(mainWindow.width * 0.2,
                                           50), 300) : 50
    z: 300

    color: "#efefef"

    Rectangle {

        visible: root.isOpen

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
            root.isAnimated = true
            root.isOpen = !root.isOpen
            borderWidth = root.isOpen ? 0 : 1

            root.isAnimated = false
        }
    }

    Behavior on width {
        enabled: root.isAnimated
        NumberAnimation {
            duration: root.animationInterval
            easing.type: Easing.OutCubic
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
        boundsBehavior: Flickable.StopAtBounds
        visible: root.isOpen
        currentIndex: -1

        model: ClientConnectionManager

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
                color: model.connectionState === 1 ? "#75e075" : "#cfcfcf"

                // visible: (activeConnectionIndex === index)
                //          && activeConnectionIndex != -1
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

                text: model.name
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

                    Client.setActiveConnection(index)
                    activeConnectionIndex = index

                    // mainPageHeader.activeConnectionName
                    //         = ClientConnectionManager.getActiveConnection().name
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

                source: "qrc:Icons/delete.svg"

                MouseArea {
                    anchors.centerIn: parent.centerIn
                    width: parent.width + 4
                    height: parent.height + 4

                    onClicked: {
                        ClientConnectionManager.deleteConnection(
                                    savedPresets.currentIndex, index)
                    }
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

                source: "qrc:Icons/plus.svg"

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

        visible: root.isOpen

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

                source: "qrc:Icons/settings.svg"

                visible: false

                smooth: true
                antialiasing: true
                mipmap: true

                fillMode: Image.PreserveAspectFit
            }
            ColorOverlay {
                anchors.fill: settings
                source: settings
                color: "#5371ad"
            }

            // onClicked: {
            //     pagesStack.push(settingsPage)
            // }
        }
    }

    Rectangle {
        visible: root.isOpen
        z: 200
        anchors {
            top: root.top
            left: root.right
            bottom: root.bottom
        }
        width: root.mainWindow.width - root.width
        color: "#000"
        opacity: root.isOpen ? 0.2 : 0

        MouseArea {
            anchors.fill: parent
            onClicked: {
                burgerButton.click()
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: root.animationInterval
                easing.type: Easing.OutCubic
            }
        }
    }
}
