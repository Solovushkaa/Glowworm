cmake_minimum_required(VERSION 3.25)
project(Glowworm VERSION 0.1 LANGUAGES CXX)

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Test Network WebSockets Xml)
find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(qml/buttons)


# === LIBS ===
# Creating all library targets
include(cmake/create_libraries.cmake)
create_libraries(APP_LIB) # Return application library targets in APP_LIB


# === APP ===
# Main target
qt_add_executable(Glowworm ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_link_libraries(Glowworm
    PUBLIC
        Qt6::Quick
        ${APP_LIB}
        CustomButtonsLib
)


# === QML ===
# Creating all qml modules
include(cmake/create_qml_modules.cmake)
create_qml_modules() # Creating qml modules


# === Resources ===
# Adding all application resources
include(cmake/add_resources.cmake)
add_resources(Glowworm)


# # === TESTS ===
# include(GoogleTest)

# # Creating all unit test targets
# include(cmake/create_unit_tests.cmake)
# file(GLOB TEST_FILES tests/unit/*.cpp)
# create_unit_tests("${TEST_FILES}")

# # Creating all functional test targets
# include(cmake/create_functional_tests.cmake)
# file(GLOB TEST_FILES tests/functional/*.cpp)
# create_functional_tests("${TEST_FILES}")

# find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
# qt_import_plugins(Glowworm INCLUDE Qt6::QWaylandIntegrationPlugin)

include(GNUInstallDirs)

install(TARGETS Glowworm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET Glowworm
    OUTPUT_SCRIPT deploy_script
)
install(SCRIPT ${deploy_script})

install(FILES ${CMAKE_SOURCE_DIR}/Content/Glowworm.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
install(FILES ${CMAKE_SOURCE_DIR}/Content/Icons/glowworm.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)
