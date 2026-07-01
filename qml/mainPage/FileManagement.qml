import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    color: "#ffffff"
    radius: 4

    property int elementHeight: root.height / 5
    property bool systemDirectory: false

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Area {
            id: downloadID
            name: systemDirectory ? "Upload" : "Download"
            isDelimiterVisible: false
            elementHeight: root.elementHeight
        }
        Area {
            id: renameID
            name: "Rename"
            elementHeight: root.elementHeight
        }
        Area {
            id: deleteID
            name: "Delete"
            isDelimiterVisible: false
            elementHeight: root.elementHeight
        }
        Area {
            id: createDirectoryID
            name: "Create directory"
            elementHeight: root.elementHeight
        }
        Area {
            id: refreshID
            name: "Refresh"
            isDelimiterVisible: false
            elementHeight: root.elementHeight
        }
    }
}
