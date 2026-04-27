cmake_minimum_required(VERSION 3.25)
project(Glowworm VERSION 0.1 LANGUAGES CXX)

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Core Quick Test Bluetooth)
find_package(GTest REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

# Creating all library targets
include(cmake/create_libraries.cmake)
create_libraries(LIB_TARGETS) # Return all library targets in LIB_TARGETS

qt_add_executable(Glowworm ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_link_libraries(Glowworm
    PUBLIC
        Qt6::Core
        Qt6::Quick
        ${LIB_TARGETS}
)

# Creating all test targets
include(cmake/create_tests.cmake)
file(GLOB TEST_FILES tests/unit/*.cpp)
create_tests("${TEST_FILES}")
