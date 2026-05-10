function(create_functional_tests TEST_FILES)
    message(STATUS "Configuring functional tests")
    qt_add_executable(FunctionalTests ${TEST_FILES})

    set_target_properties(FunctionalTests
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY
                ${CMAKE_BINARY_DIR}/tests
    )

    target_link_libraries(FunctionalTests
        PRIVATE
            Qt6::Core
            Qt6::Test
            GTest::GTest
            Client
            Server
    )
    target_include_directories(FunctionalTests
        PRIVATE
            tests/functional
    )

    gtest_discover_tests(FunctionalTests
        DISCOVERY_MODE
            PRE_TEST
        PROPERTIES
            LABELS "FunctionalTests"
        TEST_PREFIX
            "FunctionalTests."
    )
endfunction()
