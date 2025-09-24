import QtQuick
import QtQuick.Controls
import "mainPage"
import "settingsPage"
import "popups"
import SavedConnectionManager

ApplicationWindow {
    id: window
    width: Screen.width * 0.5
    height: Screen.height * 0.48
    visible: true
    title: qsTr("DFSystem")

    minimumWidth: Screen.width * 0.4
    minimumHeight: Screen.height * 0.48

    function loadConfigs() {
        ConnectionManager.connectionsLoaded()
    }

    Component.onCompleted: {
        loadConfigs()
    }

    property bool isStartAddPageVisible: true
    property bool isAddPageVisible: true

    StackView {
        id: pagesStack

        anchors.fill: parent

        initialItem: mainPage

        popEnter: null
        popExit: null
        pushEnter: null
        pushExit: null
        replaceEnter: null
        replaceExit: null
    }

    Component {
        id: mainPage
        MainPage {}
    }
    Component {
        id: settingsPage
        SettingsPage {}
    }

    StartAddPopup {
        id: startAddPopup
    }

    AddPopup {
        id: addPopup
    }
}
