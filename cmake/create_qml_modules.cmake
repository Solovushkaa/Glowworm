function(create_qml_modules)

    qt_add_qml_module(Glowworm
        URI GlowwormModule
        VERSION 1.0

        QML_FILES
            qml/Main.qml
        QML_FILES
            qml/mainPage/MainPage.qml
            qml/mainPage/BurgerMenu.qml
            qml/mainPage/Footer.qml
            qml/mainPage/FileInfoPanel.qml
            qml/mainPage/Header.qml
            qml/mainPage/Area.qml
            qml/mainPage/DownloadMenu.qml
            qml/mainPage/CustomProgressBar.qml
            qml/mainPage/DownloadStatusPanel.qml
            qml/mainPage/CustomImage.qml
            qml/mainPage/FileBrowser.qml
            qml/mainPage/FileSystemBrowser.qml
            qml/mainPage/CustomToolTip.qml


            # qml/settingsPage/SettingsPage.qml

            qml/popups/AddPopup.qml
            qml/popups/QuickKeyGenPopup.qml
            qml/popups/StartAddPopup.qml
            qml/popups/CustomTabBar.qml
            qml/popups/CustomTabButton.qml
            qml/popups/QuickAddMenu.qml
            qml/popups/DiskAddMenu.qml
            qml/popups/AdvancedAddMenu.qml

            qml/utils/Colors.qml

        IMPORTS
            CustomButtons
    )

endfunction()
