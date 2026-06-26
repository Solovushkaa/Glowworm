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
