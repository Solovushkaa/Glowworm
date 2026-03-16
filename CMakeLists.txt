cmake_minimum_required(VERSION 3.16)

set(QML_IMPORT_PATH
    "${CMAKE_BINARY_DIR}/qml/buttons"
    CACHE STRING "Path for QML modules"
)

project(DFSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

find_package(Qt6 REQUIRED COMPONENTS
    Quick
    Core
    HttpServer
    Bluetooth
)

qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(qml/buttons)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_add_executable(appDFSystem
    src/main.cpp
)

qt_add_qml_module(appDFSystem
    URI DFSystem

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
    QML_FILES
        qml/settingsPage/SettingsPage.qml

    QML_FILES
        qml/popups/AddPopup.qml
        qml/popups/StartAddPopup.qml

    SOURCES
        include/network/client/directclient.h
        src/network/client/directclient.cpp
    SOURCES
        include/network/server/httpserver.h
        src/network/server/httpserver.cpp
    SOURCES
        include/json/jsonhelperfunctions.h
        src/json/jsonhelperfunctions.cpp
    SOURCES
        include/managers/client/clientconnectionmanager.h
        src/managers/client/clientconnectionmanager.cpp
    SOURCES
        include/managers/client/downloadinfo.h
    SOURCES
        include/managers/client/downloadmanager.h
        src/managers/client/downloadmanager.cpp
    SOURCES
        include/application/initialize.h
        src/application/initialize.cpp
    SOURCES
        include/application/app.h
        src/application/app.cpp
    SOURCES
        include/managers/server/serverconnectionmanager.h
        src/managers/server/serverconnectionmanager.cpp
    SOURCES
        include/managers/client/connectioninfo.h
    SOURCES
        include/managers/client/client.h
        src/managers/client/client.cpp
    SOURCES
        include/network/client/baseclient.h
    SOURCES
        include/application/log.h
    SOURCES
        include/managers/additional.h

    RESOURCES
        Resource.qrc
        SOURCES
        SOURCES
    )


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting
# explicit, fixed bundle identifier manually though.
# set_target_properties(appDFSystem PROPERTIES
#     MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDFSystem
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )

target_include_directories(appDFSystem PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/network/server
    ${CMAKE_CURRENT_SOURCE_DIR}/include/network/client
    ${CMAKE_CURRENT_SOURCE_DIR}/include/json
    ${CMAKE_CURRENT_SOURCE_DIR}/include/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/managers/server
    ${CMAKE_CURRENT_SOURCE_DIR}/include/managers/client
    ${CMAKE_CURRENT_SOURCE_DIR}/include/managers/client/connectioninfo
    ${CMAKE_CURRENT_SOURCE_DIR}/include/application
    ${CMAKE_CURRENT_SOURCE_DIR}/include/connectors
)

target_link_libraries(appDFSystem
    PRIVATE
        Qt6::Quick
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
        Qt6::Bluetooth
        CustomButtonsLib
)

include(GNUInstallDirs)
install(TARGETS appDFSystem
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
