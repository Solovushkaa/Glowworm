function(create_unit_tests TEST_FILES)
    message(STATUS "Configuring unit tests")
    qt_add_executable(UnitTests ${TEST_FILES})

    set_target_properties(UnitTests
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY
                ${CMAKE_BINARY_DIR}/tests
    )

    target_link_libraries(UnitTests
        PRIVATE
            Qt6::Core
            Qt6::Test
            GTest::GTest
            ClientManagers
    )
    target_include_directories(UnitTests
        PRIVATE
            tests/unit
    )

    gtest_discover_tests(UnitTests
        DISCOVERY_MODE
            PRE_TEST
        PROPERTIES
            LABELS "UnitTests"
        TEST_PREFIX
            "UnitTests."
    )
endfunction()
