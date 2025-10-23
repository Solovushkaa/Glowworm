cmake_minimum_required(VERSION 3.16)

set(QML_IMPORT_PATH
    "${CMAKE_BINARY_DIR}/qml/buttons"
    CACHE STRING "Path for QML modules"
)

project(DFSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS HttpServer)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

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
        include/network/httpclient.h
        src/network/httpclient.cpp
    SOURCES
        include/network/httpserver.h
        src/network/httpserver.cpp
    SOURCES
        include/json/jsonhelperfunctions.h
        src/json/jsonhelperfunctions.cpp
    SOURCES
        include/managers/savedconnectionmanager.h
        src/managers/savedconnectionmanager.cpp
    SOURCES
        include/network/downloadinfo.h
        src/network/downloadinfo.cpp
    SOURCES
        include/managers/unfinisheddownloadmanager.h
        src/managers/unfinisheddownloadmanager.cpp
    SOURCES
        include/managers/initialize.h
        src/managers/initialize.cpp
    SOURCES
        include/application/app.h
        src/application/app.cpp
    SOURCES
        include/connectors/guihttpclientcoonnector.h
        src/connectors/guihttpclientcoonnector.cpp

    RESOURCES
        Resource.qrc
    )


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting
# explicit, fixed bundle identifier manually though.
set_target_properties(appDFSystem PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDFSystem
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appDFSystem PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/network
    ${CMAKE_CURRENT_SOURCE_DIR}/include/json
    ${CMAKE_CURRENT_SOURCE_DIR}/include/managers
)

target_link_libraries(appDFSystem
    PRIVATE
        Qt6::Quick
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
        CustomButtonsLib
)
target_link_libraries(appDFSystem PRIVATE Qt6::Quick)
target_link_libraries(appDFSystem PRIVATE Qt6::Core)
target_link_libraries(appDFSystem PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS appDFSystem
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
