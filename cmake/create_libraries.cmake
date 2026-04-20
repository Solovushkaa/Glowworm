function(create_libraries)
    set(SRC_PATHS "")
    set(INC_PATHS "")
    list(APPEND SRC_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp)
    list(APPEND INC_PATHS ${CMAKE_SOURCE_DIR}/include/managers/client/downloadinfo.hpp)

    # list(APPEND LIB_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp)
    # list(APPEND LIB_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp)

    # list(APPEND LIB_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp)
    # list(APPEND LIB_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp)

    # list(APPEND LIB_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp)
    # list(APPEND LIB_PATHS ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp)


    set(LIB_TARGETS "" PARENT_SCOPE)
    set(INC_TARGETS "" PARENT_SCOPE)

    foreach(SRC INC IN ZIP_LISTS SRC_PATHS INC_PATHS)
        get_filename_component(SRC_NAME_WE "${SRC}" NAME_WE)
        get_filename_component(INC_NAME "${INC}" NAME)

        qt_add_library(${SRC_NAME_WE}
            STATIC
                ${SRC}
                ${INC}
        )

        get_filename_component(INC_PATH "${INC}" PATH)
        target_include_directories(${SRC_NAME_WE} PUBLIC ${INC_PATH})
        target_link_libraries(${SRC_NAME_WE} PRIVATE Qt6::Core)
        target_compile_features(${SRC_NAME_WE} PRIVATE cxx_std_20)

        set_target_properties(${SRC_NAME_WE}
            PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY
                    ${CMAKE_BINARY_DIR}/libs
                ARCHIVE_OUTPUT_DIRECTORY
                    ${CMAKE_BINARY_DIR}/libs
        )

        LIST(APPEND LIB_TARGETS ${SRC_NAME_WE})
        LIST(APPEND INC_TARGETS ${INC_PATH})

    endforeach()

endfunction()
