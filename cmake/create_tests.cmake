function(create_tests TEST_FILES)
    include(GoogleTest)

    message(STATUS "Configuring tests")
    qt_add_executable(GlowwormTests ${TEST_FILES})

    set_target_properties(GlowwormTests
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY
                ${CMAKE_BINARY_DIR}/tests
    )

    target_link_libraries(GlowwormTests
        PRIVATE
            Qt6::Core
            Qt6::Test
            GTest::GTest
            ClientManagers
    )
    target_include_directories(GlowwormTests
        PRIVATE
            tests/unit/
    )

    gtest_discover_tests(GlowwormTests
        DISCOVERY_MODE
            PRE_TEST
    )
endfunction()
