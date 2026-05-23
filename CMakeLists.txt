cmake_minimum_required(VERSION 3.25)
project(Glowworm VERSION 0.1 LANGUAGES CXX)

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Test Network HttpServer)
find_package(GTest REQUIRED)

qt_standard_project_setup(REQUIRES 6.9)

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


# === TESTS ===
include(GoogleTest)

# Creating all unit test targets
include(cmake/create_unit_tests.cmake)
file(GLOB TEST_FILES tests/unit/*.cpp)
create_unit_tests("${TEST_FILES}")

# Creating all functional test targets
include(cmake/create_functional_tests.cmake)
file(GLOB TEST_FILES tests/functional/*.cpp)
create_functional_tests("${TEST_FILES}")
