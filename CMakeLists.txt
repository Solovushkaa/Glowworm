cmake_minimum_required(VERSION 3.26)
project(Glowworm VERSION 0.1 LANGUAGES CXX)

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Core Quick Test)
find_package(GTest REQUIRED)

qt_standard_project_setup()

# Creating all library targets
include(cmake/create_libraries.cmake)
create_libraries() # Return all library targets (${LIB_TARGETS})

qt_add_executable(Glowworm ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_include_directories(Glowworm
    PRIVATE
        ${INC_TARGETS}
)
target_link_libraries(Glowworm
    PRIVATE
        Qt6::Core
        Qt6::Quick
        ${LIB_TARGETS}
)

# Creating all test targets
include(cmake/create_tests.cmake)
file(GLOB TEST_FILES tests/unit/*.cpp)
create_tests("${TEST_FILES}")
