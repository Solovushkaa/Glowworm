cmake_minimum_required(VERSION 3.28)
project(Glowworm VERSION 0.1 LANGUAGES CXX)

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Core Test)
include(GoogleTest)

qt_standard_project_setup()

qt_add_executable(Unit 
    tests/unit/downloadinfo/testdownloadinfo.cpp 
    include/managers/client/downloadinfo.cpp
)

# set_target_properties(Unit PROPERTIES
#     AUTOMOC ON
#     AUTOUIC ON
#     AUTORCC ON
# )

target_include_directories(Unit PRIVATE include/managers/client)

target_link_libraries(Unit PRIVATE Qt6::Core Qt6::Test gtest)

target_compile_features(Unit PRIVATE cxx_std_23)

# add_test(NAME first_test COMMAND Unit)
gtest_discover_tests(Unit DISCOVERY_MODE PRE_TEST)