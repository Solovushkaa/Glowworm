import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Effects

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

    property bool isSelectActive: false

    onIsOpenChanged: {
        burgerMenuOpen = root.isOpen
        console.log("Burger menu open:", burgerMenuOpen)
    }

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
    width: root.isOpen ? Math.min(Math.max(mainWindow.width * 0.3,
                                           50), 300) : 50
    z: 300

    color: root.isOpen ? "#ffffff" : "#efefef"

    Behavior on color {
        ColorAnimation {
            duration: root.animationInterval
        }
    }

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
            topMargin: 4
            bottomMargin: 10
        }

        clip: true
        boundsBehavior: Flickable.StopAtBounds
        visible: root.isOpen
        currentIndex: -1

        model: ClientConnectionManager

        delegate: Rectangle {
            id: delegateID

            property bool isSelect: false

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 50

            color: ListView.isCurrentItem ? "#efefef" : "transparent"

            radius: 4

            // Rectangle {
            //     anchors {
            //         top: parent.top
            //         horizontalCenter: parent.horizontalCenter
            //         topMargin: -1
            //     }
            //     width: parent.width * 0.75
            //     height: 1
            //     opacity: 0.3
            //     color: "#3d5482"
            // }
            Rectangle {
                anchors {
                    left: parent.left
                    top: parent.top
                    topMargin: 5
                    bottom: parent.bottom
                    bottomMargin: 5
                }

                width: 3
                color: model.connectionState === 1 ? "#3dbf5c" : "#cfcfcf"

                radius: 2
            }

            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: delimiterID.left
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
                id: delimiterID

                anchors {
                    bottom: parent.bottom
                    // bottomMargin: 1
                    horizontalCenter: parent.horizontalCenter
                }
                width: parent.width * 0.9
                height: 1

                color: "#efefef"
                radius: 2

                visible:
                    /*!ListView.isCurrentItem
                         || */ (savedPresets.currentIndex !== index + 1)
            }

            Rectangle {
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: 4
                }
                width: 20
                height: 20

                radius: 4

                border {
                    width: 2
                    color: "#5371ad"
                }

                visible: root.isSelectActive

                // CustomImage {
                //     anchors.fill: parent

                //     source: "qrc:Icons/check.svg"

                //     visible: delegateID.isSelect
                // }
                Rectangle {
                    anchors.fill: parent

                    visible: delegateID.isSelect

                    color: "transparent"

                    Image {
                        id: check

                        anchors {
                            fill: parent
                        }

                        source: "qrc:Icons/check.svg"
                        sourceSize.width: 200
                        sourceSize.height: 200

                        visible: false

                        // smooth: true
                        antialiasing: true
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                    }

                    ColorOverlay {
                        anchors.fill: check
                        source: check
                        color: "#5371ad"
                    }
                }

                MouseArea {
                    anchors.centerIn: parent
                    width: parent.width + 4
                    height: parent.height + 4

                    onClicked: {
                        delegateID.isSelect = !delegateID.isSelect
                    }
                }
            }

            MouseArea {
                id: mouse

                anchors.fill: parent

                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked: function (mouse) {
                    savedPresets.currentIndex = index

                    if (mouse.button === Qt.RightButton) {
                        root.isSelectActive = !root.isSelectActive
                    }
                }
                onDoubleClicked: function (mouse) {
                    if (mouse.button === Qt.LeftButton) {
                        savedPresets.currentIndex = index

                        // currentPath = "/"
                        Client.setActiveConnection(index)
                        activeConnectionIndex = index

                        // mainPageHeader.activeConnectionName
                        //         = ClientConnectionManager.getActiveConnection().name
                    }
                }

                CustomToolTip {
                    id: tooltipRi

                    visibilityController: mouse
                    visibilityMode: root.isSelectActive && delegateID.isSelect

                    delay: 0

                    onClosed: {
                        root.isSelectActive = false
                        mouse.isSelect = false
                    }
                    contentItem: FileManagement {

                        width: Screen.width * 0.08 - 2
                        height: Screen.height * 0.15 - 2

                        anchors.centerIn: parent
                        opacity: tooltipRi.visible ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutCubic
                            }
                        }
                    }

                    onVisibleChanged: {
                        delegateID.isSelect = false
                    }
                }
            }
            // CustomImage {
            //     anchors {
            //         top: parent.top
            //         right: parent.right
            //         margins: 4
            //     }
            //     width: 15
            //     height: 15

            //     source: "qrc:Icons/delete.svg"

            //     MouseArea {
            //         anchors.centerIn: parent.centerIn
            //         width: parent.width + 4
            //         height: parent.height + 4

            //         onClicked: {
            //             ClientConnectionManager.deleteConnection(
            //                         savedPresets.currentIndex, index)
            //         }
            //     }
            // }
        }

        footer: Rectangle {
            width: 43
            height: width

            color: "transparent"

            Image {
                id: plus

                anchors {
                    fill: parent
                    topMargin: 10
                }

                source: "qrc:Icons/plus.svg"
                sourceSize.width: 300
                sourceSize.height: 300

                visible: false

                // smooth: true
                antialiasing: true
                mipmap: true
                fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
                anchors.fill: plus
                source: plus
                color: "#5371ad"
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
            margins: 5
        }

        width: 35
        height: width

        color: "transparent"

        Button {
            anchors {
                fill: parent
            }

            background: Rectangle {
                anchors.fill: parent
                color: "transparent"
            }

            Image {
                id: settings

                anchors.fill: parent

                source: "qrc:Icons/settings.svg"
                sourceSize.width: 300
                sourceSize.height: 300

                visible: false

                // smooth: true
                antialiasing: true
                mipmap: true

                fillMode: Image.PreserveAspectFit
            }
            ColorOverlay {
                anchors.fill: settings
                source: settings
                color: "#5371ad"
            }

            onClicked: {
                pagesStack.push(settingsPage)
            }
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
