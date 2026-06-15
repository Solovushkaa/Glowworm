import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

TabBar {
    id: tabBar
    Layout.fillWidth: true
    Layout.preferredHeight: 40

    background: Rectangle {
        id: customRect
        color: "#ffffff"
        border {
            width: 1
            color: "#dddddd"
        }
        radius: quickTab.radius

        // Shadow
        MultiEffect {
            source: customRect
            anchors.fill: customRect
            shadowEnabled: true
            shadowBlur: 0.5
            shadowHorizontalOffset: 2
            shadowVerticalOffset: 2
            shadowColor: "#20000000"
        }
    }

    // First tab
    CustomTabButton {
        id: quickTab
        buttonText: "Quick"
    }

    // Second tab
    CustomTabButton {
        buttonText: "Disk (WebDAV)"
    }

    // Third tab
    CustomTabButton {
        buttonText: "Advanced"
    }
}
