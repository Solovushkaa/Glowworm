import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    color: "#ffffff"
    radius: 4

    property int elementHeight: root.height / 6

    property string name: ""
    property string typeName: ""
    property string locationName: ""
    property string sizeName: ""
    property string createdName: ""
    property string modifiedName: ""
    property string accessedName: ""

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        // Rectangle {
        //     id: nameID

        //     Layout.alignment: Qt.AlignTop | Qt.AlignLeft
        //     Layout.preferredHeight: elementHeight
        //     Layout.fillWidth: true

        //     color: "#cccccc"
        //     radius: 4

        //     Rectangle {
        //         anchors {
        //             left: parent.left
        //             right: parent.right
        //             bottom: parent.bottom
        //         }
        //         height: elementHeight / 2

        //         color: "#cccccc"
        //     }
        //     Rectangle {
        //         anchors {
        //             left: parent.left
        //             right: parent.right
        //             bottom: parent.bottom
        //         }

        //         height: 1

        //         color: "#bfbfbf"
        //     }

        //     Text {
        //         id: nameTextID
        //         anchors {
        //             left: parent.left
        //             right: parent.right
        //             top: parent.top
        //             bottom: parent.bottom
        //             margins: 5
        //         }

        //         horizontalAlignment: Text.AlignHCenter
        //         verticalAlignment: Text.AlignVCenter

        //         font.pointSize: 12

        //         wrapMode: Text.WrapAnywhere

        //         maximumLineCount: 2
        //         elide: Text.ElideRight

        //         clip: true

        //         text: root.typeName + " " + root.name
        //     }
        // }

        // Area {
        //     id: typeID
        //     isDelimiterVisible: false
        //     prefix: "Type: "
        //     name: typeName
        //     elementHeight: root.elementHeight
        // }
        Area {
            id: nameID
            prefix: ""
            name: root.typeName + " " + root.name
            isDelimiterVisible: false
            isAlignHCenter: true
            elementHeight: root.elementHeight
        }
        Area {
            id: locationID
            prefix: "Location: "
            name: locationName
            elementHeight: root.elementHeight
        }
        Area {
            id: sizeID
            prefix: "Size: "
            name: sizeName
            postfix: name === "" ? (typeName === "Directory" ? "-" : "") : "B"
            elementHeight: root.elementHeight
        }
        Area {
            id: createdID
            prefix: "Created: "
            name: createdName
            elementHeight: root.elementHeight
        }
        Area {
            id: modifiedID
            prefix: "Modified: "
            name: modifiedName
            elementHeight: root.elementHeight
        }
        Area {
            id: accessedID
            prefix: "Accessed: "
            name: accessedName
            elementHeight: root.elementHeight
        }
    }
}
