function(create_libraries LIB_TARGETS)
    set(SRC_PATHS "")
    set(INC_PATHS "")
    set(ADDITIONAL_TARGETS "")
    list(APPEND SRC_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp)
    list(APPEND INC_PATHS ${CMAKE_SOURCE_DIR}/include/managers/client/downloadinfo.hpp)
    set(DI_TARGETS "Qt6::Core")
    list(APPEND ADDITIONAL_TARGETS DI_TARGETS)

    list(APPEND SRC_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadmanager.cpp)
    list(APPEND INC_PATHS ${CMAKE_SOURCE_DIR}/include/managers/client/downloadmanager.hpp)
    set(DM_TARGETS "downloadinfo")
    list(APPEND ADDITIONAL_TARGETS DM_TARGETS)

    set(LIB_TARGETS "")
    foreach(SRC INC TRG IN ZIP_LISTS SRC_PATHS INC_PATHS ADDITIONAL_TARGETS)
        get_filename_component(SRC_NAME_WE "${SRC}" NAME_WE)
        get_filename_component(INC_NAME "${INC}" NAME)

        message(STATUS "Configure library target: " ${SRC_NAME_WE})
        message(STATUS "ADDITIONAL_TARGETS: " ${${TRG}})

        qt_add_library(${SRC_NAME_WE}
            OBJECT
                ${SRC}
                ${INC}
        )

        get_filename_component(INC_PATH "${INC}" PATH)
        target_include_directories(${SRC_NAME_WE} PUBLIC ${INC_PATH})
        target_link_libraries(${SRC_NAME_WE} PUBLIC ${${TRG}})
        target_compile_features(${SRC_NAME_WE} PUBLIC cxx_std_20)

        # set_target_properties(${SRC_NAME_WE}
        #     PROPERTIES
        #         LIBRARY_OUTPUT_DIRECTORY
        #             ${CMAKE_BINARY_DIR}/libs
        #         ARCHIVE_OUTPUT_DIRECTORY
        #             ${CMAKE_BINARY_DIR}/libs
        # )

        LIST(APPEND LIB_TARGETS ${SRC_NAME_WE})

    endforeach()

    return(PROPAGATE LIB_TARGETS)

endfunction()
