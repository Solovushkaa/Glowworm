import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    Layout.fillWidth: true
    Layout.preferredHeight: {
        let listMargins = 20
        let listHeight = settingList.contentHeight
        return listMargins + listHeight
    }

    property string areaName: ""
    property ListModel settings: null

    radius: 8

    border {
        color: "#cccccc"
        width: 1
    }

    color: "#ffffff"

    Rectangle {

        width: areaNameID.implicitWidth + 10
        height: areaNameID.implicitHeight

        anchors {
            top: parent.top
            topMargin: -height / 2
            left: parent.left
            leftMargin: 10
        }

        color: "#ffffff"

        Text {
            id: areaNameID

            anchors.centerIn: parent

            text: root.areaName

            font.pointSize: 14
            renderTypeQuality: Text.HighRenderTypeQuality
            renderType: Text.NativeRendering
        }
    }

    ListView {
        id: settingList

        anchors {
            top: parent.top
            // topMargin: 20
            left: parent.left
            right: parent.right
            margins: 10
        }
        height: contentHeight

        model: root.settings

        interactive: false

        delegate: Rectangle {
            id: delegateID

            height: 40
            width: parent.width

            Text {
                id: prefix

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                text: model.prefix
                font.pointSize: 12
                renderTypeQuality: Text.VeryVeryHighRenderTypeQuality
                font.hintingPreference: Font.PreferFullHinting
                // renderType: Text.NativeRendering
            }

            TextArea {
                id: value

                visible: !model.isSwitch

                anchors {
                    left: prefix.right
                    leftMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                text: model.value
                color: "black"
                font.pointSize: 12
                font.hintingPreference: Font.PreferFullHinting
                // renderTypeQuality: Text.VeryHighRenderTypeQuality
                // renderType: Text.NativeRendering
                background: Rectangle {

                    anchors {
                        centerIn: parent
                    }

                    height: value.implicitHeight * 0.9
                    width: value.implicitWidth

                    radius: 5

                    border {
                        width: 1
                        color: "#bbbbbb"
                    }
                }
            }
            Switch {
                id: switchID

                visible: model.isSwitch

                height: 27
                width: 65
                anchors {
                    left: prefix.right
                    leftMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                checked: SecurityOptions.sessionKeyEnabled() // TODO:

                indicator: Rectangle {
                    width: parent.width
                    height: parent.height
                    radius: 20

                    color: switchID.checked ? "#5371ad" : "#e0e0e0"

                    Behavior on color {
                        ColorAnimation {
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Text {
                        visible: switchID.checked

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
                        visible: !switchID.checked

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

                        x: switchID.checked ? parent.width - width - 2 : 2

                        Behavior on x {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                }

                contentItem: Text {
                    text: switchID.text
                    font.pointSize: 11
                    leftPadding: switchID.indicator.width + 8
                    verticalAlignment: Text.AlignVCenter
                    visible: switchID.text !== ""
                }
            }

            Text {
                id: postfix

                anchors {
                    left: value.right
                    leftMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                text: model.postfix
                font.pointSize: 12
                renderTypeQuality: Text.VeryHighRenderTypeQuality
                font.hintingPreference: Font.PreferFullHinting
                renderType: Text.NativeRendering
            }

            // Rectangle {
            //     height: 15
            //     width: height
            //     anchors {
            //         right: parent.right
            //         rightMargin: 5
            //         verticalCenter: parent.verticalCenter
            //     }
            //     color: "red"
            // }
            Button {
                height: parent.height * 0.45
                width: height
                anchors {
                    right: parent.right
                    rightMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                }
                Image {
                    id: reload

                    anchors.fill: parent

                    source: "qrc:Icons/reload.svg"

                    visible: false
                    smooth: true
                    antialiasing: true
                    mipmap: true

                    fillMode: Image.PreserveAspectFit
                }
                ColorOverlay {
                    anchors.fill: reload
                    source: reload
                    // color: "#5371ad"
                    color: "black"
                }
            }
        }
    }
}
