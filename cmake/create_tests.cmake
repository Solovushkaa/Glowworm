function(create_tests TEST_FILES)
    include(GoogleTest)

    message(STATUS ${TEST_FILES})
    qt_add_executable(ClientManagersTests ${TEST_FILES})

    set_target_properties(ClientManagersTests
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY
                ${CMAKE_BINARY_DIR}/tests
    )

    target_link_libraries(ClientManagersTests
        PRIVATE
            Qt6::Core
            Qt6::Test
            GTest::GTest
            downloadinfo
            downloadmanager
    )
    target_include_directories(ClientManagersTests
        PRIVATE
            include/managers/client
            tests/unit/
    )

    target_compile_features(ClientManagersTests PRIVATE cxx_std_23)
    target_compile_definitions(ClientManagersTests PRIVATE QT_NO_DEBUG_OUTPUT)

    gtest_discover_tests(ClientManagersTests
        DISCOVERY_MODE
            PRE_TEST
    )

    # foreach()
    # endforeach()

    # foreach(TEST_FILE ${TEST_FILES})
    #     get_filename_component(TARGET_NAME "${TEST_FILE}" NAME_WE)
    #     message(STATUS "Configure test target: " ${TARGET_NAME})

    #     set(TEST_TARGET_NAME t_${TARGET_NAME})

    #     qt_add_executable(${TEST_TARGET_NAME}
    #         "${CMAKE_SOURCE_DIR}/tests/unit/${TARGET_NAME}.cpp"
    #     )

    #     set_target_properties(${TEST_TARGET_NAME}
    #         PROPERTIES
    #             RUNTIME_OUTPUT_DIRECTORY
    #                 ${CMAKE_BINARY_DIR}/tests
    #     )

    #     target_link_libraries(${TEST_TARGET_NAME}
    #         PRIVATE
    #             ${TARGET_NAME}
    #             Qt6::Core
    #             Qt6::Test
    #             # $<$<IN_LIST:Qt6::Core,$<TARGET_PROPERTY:${TARGET_NAME},INTERFACE_LINK_LIBRARIES>>:Qt6::Test>
    #             GTest::GTest
    #     )
    #     target_include_directories(${TEST_TARGET_NAME} PRIVATE ${TARGET_NAME})

    #     target_compile_features(${TEST_TARGET_NAME} PRIVATE cxx_std_20)

    #     gtest_discover_tests(${TEST_TARGET_NAME} DISCOVERY_MODE PRE_TEST)
    #     # set_tests_properties(${TEST_TARGET_NAME} PROPERTIES DEPENDS  ${TARGET_NAME})
    # endforeach()
endfunction()
