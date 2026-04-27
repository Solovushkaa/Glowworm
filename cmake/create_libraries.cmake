function(create_libraries LIB_TARGETS)
    set(LIB_TARGETS "")

    qt_add_library(ClientManagers STATIC)
    target_sources(ClientManagers PRIVATE
        ${CMAKE_SOURCE_DIR}/src/managers/client/downloadinfo.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/downloadinfo.hpp
        ${CMAKE_SOURCE_DIR}/src/managers/client/downloadmanager.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/downloadmanager.hpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/connectioninfo.hpp
        ${CMAKE_SOURCE_DIR}/src/managers/client/clientconnectionmanager.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/clientconnectionmanager.hpp
    )
    target_include_directories(ClientManagers
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/managers/client
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
