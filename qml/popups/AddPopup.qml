import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CustomButtons

Popup {
    id: root

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
