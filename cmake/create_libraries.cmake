function(create_libraries LIB_TARGETS)
    set(LIB_TARGETS "")

    qt_add_library(ClientManagers STATIC)
    target_sources(ClientManagers PRIVATE
        ${CMAKE_SOURCE_DIR}/src/managers/client/download_info.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/download_info.hpp
        ${CMAKE_SOURCE_DIR}/src/managers/client/download_manager.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/download_manager.hpp
        ${CMAKE_SOURCE_DIR}/src/managers/client/connection_info.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/connection_info.hpp
        ${CMAKE_SOURCE_DIR}/src/managers/client/client_connection_manager.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/client_connection_manager.hpp
        ${CMAKE_SOURCE_DIR}/src/utils/json_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/json_utils.hpp
        ${CMAKE_SOURCE_DIR}/src/utils/manager_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/manager_utils.hpp
    )
    target_include_directories(ClientManagers
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/managers/client
            ${CMAKE_SOURCE_DIR}/include/managers
            ${CMAKE_SOURCE_DIR}/include/utils
    )
    target_link_libraries(ClientManagers PUBLIC Qt6::Core Qt6::Bluetooth)
    LIST(APPEND LIB_TARGETS ClientManagers)


    foreach(LIB IN LISTS LIB_TARGETS)
        target_compile_features(${LIB} PUBLIC cxx_std_23)
        target_compile_definitions(${LIB} PRIVATE QT_NO_DEBUG_OUTPUT)

        set_target_properties(${LIB}
            PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY
                    ${CMAKE_BINARY_DIR}/libs
                ARCHIVE_OUTPUT_DIRECTORY
                    ${CMAKE_BINARY_DIR}/libs
        )
    endforeach()

    return(PROPAGATE LIB_TARGETS)

endfunction()
