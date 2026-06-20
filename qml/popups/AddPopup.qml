import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CustomButtons

Popup {
    id: root

    property bool isPersistentConnection: persistentConnection.checked

    function getTextOrPlaceholder(textArea) {
        return parseInt(
                    textArea.text !== "" ? textArea.text : textArea.placeholderText)
    }

    anchors {
        centerIn: parent
    }

    // width: parent.width * 0.4
    // height: parent.height * 0.42
    padding: 10

    modal: true
    focus: true

    closePolicy: Popup.CloseOnEscape

    background: Rectangle {
        color: "white"

        radius: 30
    }

    // implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    // implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
    width: parent.width * 0.4
    implicitHeight: layout.implicitHeight + topPadding + bottomPadding

    ColumnLayout {
        id: layout

        // anchors.centerIn: parent
        anchors {
            fill: parent
            margins: 10
        }
        spacing: 10

        Text {
            Layout.alignment: Qt.AlignHCenter

            font.pointSize: 12
            text: "Add Connection"
        }

        CustomTabBar {
            id: tabBar
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            Text {
                Layout.alignment: Qt.AlignLeft
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                font.pointSize: 12

                text: "Persistent connection:"
            }

            Switch {
                id: persistentConnection

                height: parent.height * 0.8
                width: 65
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                checked: true

                indicator: Rectangle {
                    width: parent.width
                    height: parent.height
                    radius: 20

                    color: persistentConnection.checked ? "#5371ad" : "#e0e0e0"

                    Behavior on color {
                        ColorAnimation {
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Text {
                        visible: persistentConnection.checked

                        text: "ON"
                        font.bold: true
                        font.pixelSize: 12
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
                        visible: !persistentConnection.checked

                        text: "OFF"
                        font.bold: true
                        font.pixelSize: 12
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

                        x: persistentConnection.checked ? parent.width - width - 2 : 2

                        Behavior on x {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                }

                contentItem: Text {
                    text: persistentConnection.text
                    font.pointSize: 11
                    leftPadding: persistentConnection.indicator.width + 8
                    verticalAlignment: Text.AlignVCenter
                    visible: persistentConnection.text !== ""
                }
            }

            Rectangle {
                anchors {
                    bottom: parent.bottom
                    bottomMargin: -6
                    horizontalCenter: parent.horizontalCenter
                }
                height: 1
                width: parent.width * 0.85

                color: "#efefef"
                radius: 2
            }
        }

        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 15
            currentIndex: tabBar.currentIndex

            QuickAddMenu {
                id: quickAddMenu
            }
            DiskAddMenu {
                id: diskAddMenu
            }
            AdvancedAddMenu {
                id: advancedAddMenu
            }
        }
    }
}
