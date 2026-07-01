import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

Page {
    id: root

    property string active: "Network"

    anchors.fill: parent

    background: Rectangle {
        color: "#eeeeee"
    }

    ColumnLayout {

        anchors {
            fill: parent
            margins: 5
            // topMargin: 30
        }

        RowLayout {
            id: mainLayer

            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: 5

            ColumnLayout {

                Layout.preferredWidth: 150
                Layout.fillHeight: true

                Item {
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 30
                }

                Rectangle {
                    id: settingCategory

                    Layout.preferredWidth: 150
                    Layout.fillHeight: true

                    color: "#ffffff"

                    radius: 5

                    border {
                        width: 1
                        color: "#cccccc"
                    }
                    layer.enabled: true
                    layer.effect: MultiEffect {
                        shadowEnabled: true
                        shadowBlur: 0.5
                        // shadowHorizontalOffset: 2
                        // shadowVerticalOffset: 2
                        shadowColor: "#20000000"
                    }

                    ListModel {
                        id: settingsList

                        ListElement {
                            name: "Network"
                        }
                        ListElement {
                            name: "Privacy"
                        }
                    }

                    ListView {
                        id: settingsListView

                        anchors {
                            fill: parent
                            margins: 3
                        }

                        model: settingsList

                        delegate: Rectangle {

                            radius: 5

                            width: parent.width
                            height: 30

                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }

                            // border {
                            //     width: 1
                            //     color: "#cccccc"
                            // }
                            color: ListView.isCurrentItem ? (settingsListView.activeFocus ? "lightgrey" : "#e9e9e9") : "white"

                            Text {
                                anchors {
                                    centerIn: parent
                                }

                                text: model.name
                                font.pointSize: 12
                                renderTypeQuality: Text.VeryHighRenderTypeQuality
                            }

                            MouseArea {
                                id: mouseArea

                                anchors.fill: parent

                                onClicked: {
                                    settingsListView.currentIndex = index
                                    root.active = model.name
                                }
                            }
                        }
                    }

                    // ColumnLayout {
                    //     id: settingList

                    //     anchors {
                    //         fill: parent
                    //         margins: 3
                    //     }

                    //     Area {
                    //         id: networkSettings
                    //         name: "Network"
                    //         isDelimiterVisible: false
                    //         isAlignHCenter: true
                    //         Layout.preferredHeight: 30
                    //     }
                    //     Area {
                    //         id: privacySettings
                    //         name: "Privacy"
                    //         isAlignHCenter: true
                    //         Layout.preferredHeight: 30
                    //     }

                    //     Item {
                    //         Layout.fillWidth: true
                    //         Layout.fillHeight: true
                    //     }
                    // }
                }
            }

            ColumnLayout {
                id: settingAdditions

                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle {
                    id: settingName

                    Layout.fillWidth: true
                    Layout.preferredHeight: 30

                    color: "transparent"

                    Text {
                        text: active

                        anchors {
                            bottom: parent.bottom
                            left: parent.left
                            leftMargin: 5
                        }

                        font.pointSize: 14

                        // renderTypeQuality: Text.HighRenderTypeQuality
                        renderType: Text.NativeRendering
                    }
                }

                Rectangle {
                    id: settings

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    color: "#ffffff"

                    radius: 5

                    ColumnLayout {
                        anchors {
                            fill: parent
                            margins: 15
                        }

                        visible: active === "Network"

                        spacing: 15

                        SettingArea {
                            areaName: "Server"
                            settings: ListModel {
                                ListElement {
                                    prefix: "Server messenger port:"
                                    value: "6115"
                                    postfix: ""
                                    isSwitch: false
                                }
                                ListElement {
                                    prefix: "Server transport port:"
                                    value: "1274"
                                    postfix: ""
                                    isSwitch: false
                                }
                            }
                        }
                        SettingArea {
                            areaName: "Timeout"
                            settings: ListModel {
                                ListElement {
                                    prefix: "Reconnect timeout:"
                                    value: "5"
                                    postfix: "second"
                                    isSwitch: false
                                }
                            }
                        }
                        Item {
                            Layout.fillHeight: true
                        }
                    }

                    ColumnLayout {
                        anchors {
                            fill: parent
                            margins: 15
                        }

                        visible: active === "Privacy"

                        spacing: 15

                        SettingArea {
                            areaName: "Session key"
                            settings: ListModel {
                                ListElement {
                                    prefix: "Enable session key:"
                                    postfix: ""
                                    isSwitch: true
                                }
                            }
                        }
                        Item {
                            Layout.fillHeight: true
                        }
                    }

                    border {
                        width: 1
                        color: "#cccccc"
                    }
                    layer.enabled: true
                    layer.effect: MultiEffect {
                        shadowEnabled: true
                        shadowBlur: 0.5
                        // shadowHorizontalOffset: 2
                        // shadowVerticalOffset: 2
                        shadowColor: "#20000000"
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            Layout.alignment: Qt.AlignRight

            CustomButton {
                Layout.preferredHeight: 30
                Layout.preferredWidth: 80
                buttonText: "Ok"
                onClicked: {
                    pagesStack.pop()
                }
            }
            CustomButton {
                Layout.preferredHeight: 30
                Layout.preferredWidth: 80
                buttonText: "Apply"
                onClicked: {

                    // Save settings
                }
            }
            CustomButton {
                Layout.preferredHeight: 30
                Layout.preferredWidth: 80
                buttonText: "Cancel"
                onClicked: {
                    pagesStack.pop()
                }
            }
        }
    }

    // Rectangle {
    //     anchors.fill: parent

    //     color: "white"

    //     Button {
    //         anchors {
    //             left: parent.left
    //             leftMargin: 5
    //             top: parent.top
    //             topMargin: 5
    //         }

    //         height: 40
    //         width: 40

    //         background: Image {
    //             anchors {
    //                 fill: parent
    //                 margins: 10
    //             }

    //             source: "qrc:/Content/Icons/close.svg"

    //             fillMode: Image.PreserveAspectFit
    //         }

    //         onClicked: {
    //             pagesStack.pop()
    //         }
    //     }
    // }
}
