import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    property string prefix: ""
    property string name: ""
    property string postfix: ""
    property bool isDelimiterVisible: true

    Layout.alignment: Qt.AlignTop | Qt.AlignLeft
    Layout.preferredHeight: elementHeight
    Layout.fillWidth: true

    radius: 4

    Rectangle {
        anchors {
            top: parent.top
            topMargin: -1
            horizontalCenter: parent.horizontalCenter
        }
        height: 1
        width: parent.width * 0.85

        color: "#efefef"
        radius: 2

        visible: isDelimiterVisible
    }

    Flickable {
        id: scrollZone
        anchors {
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        height: textContent.height

        contentWidth: textContent.width
        contentHeight: textContent.height

        flickableDirection: Flickable.HorizontalFlick
        boundsBehavior: Flickable.StopAtBounds

        clip: true

        Text {
            id: textContent

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            font.pointSize: 12

            textFormat: Text.RichText

            text: "<font color='#505050'>" + prefix
                  + "</font><font color='#000000'>" + name + postfix + "</font>"
        }

        ScrollBar.horizontal: ScrollBar {
            parent: scrollZone.parent
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 10
        }
    }

    TextEdit {
        id: copyToClipBoard
        text: name + postfix
        visible: false
    }

    Rectangle {
        anchors {
            top: parent.top
            topMargin: 3
            right: parent.right
            rightMargin: 3
        }

        height: 10
        width: 10

        color: "transparent"

        Image {
            id: copy

            anchors.fill: parent

            source: "qrc:Icons/copy.svg"

            visible: false

            smooth: true
            antialiasing: true
            mipmap: true

            fillMode: Image.PreserveAspectFit
        }
        ColorOverlay {
            anchors.fill: copy
            source: copy
            color: "#3d5482"
        }

        MouseArea {
            anchors.centerIn: parent.centerIn

            height: 13
            width: 13

            onClicked: {
                copyToClipBoard.selectAll()
                copyToClipBoard.copy()
            }
        }
    }
}
