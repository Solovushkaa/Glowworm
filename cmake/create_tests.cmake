function(create_tests TEST_FILES)
    include(GoogleTest)

    foreach(TEST_FILE ${TEST_FILES})
        get_filename_component(TARGET_NAME "${TEST_FILE}" NAME_WE)
        message(STATUS "Configure test target: " ${TARGET_NAME})

        set(TEST_TARGET_NAME t_${TARGET_NAME})

        qt_add_executable(${TEST_TARGET_NAME}
            "${CMAKE_SOURCE_DIR}/tests/unit/${TARGET_NAME}.cpp"
        )

        set_target_properties(${TEST_TARGET_NAME}
            PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY
                    ${CMAKE_BINARY_DIR}/tests
        )

        target_link_libraries(${TEST_TARGET_NAME}
            PRIVATE
                ${TARGET_NAME}
                Qt6::Core
                Qt6::Test
                GTest::GTest
        )

        target_compile_features(${TEST_TARGET_NAME} PRIVATE cxx_std_20)

        gtest_discover_tests(${TEST_TARGET_NAME} DISCOVERY_MODE PRE_TEST)
    endforeach()
endfunction()
