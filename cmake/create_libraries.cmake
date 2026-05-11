function(create_libraries LIB_TARGETS)
    set(LIB_TARGETS "")

    # Client managers lib
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
            ${CMAKE_SOURCE_DIR}/include/utils
            ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(ClientManagers PUBLIC Qt6::Core Qt6::Bluetooth Qt6::Qml)
    LIST(APPEND LIB_TARGETS ClientManagers)

    # Server lib
    qt_add_library(Server STATIC)
    target_sources(Server PRIVATE
        ${CMAKE_SOURCE_DIR}/src/managers/server/server.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/server/server.hpp

        ${CMAKE_SOURCE_DIR}/include/network/server/server_messenger.hpp
        ${CMAKE_SOURCE_DIR}/src/network/server/server_http_messenger.cpp
        ${CMAKE_SOURCE_DIR}/include/network/server/server_http_messenger.hpp

        # ${CMAKE_SOURCE_DIR}/include/network/server/server_transport.hpp
        # ${CMAKE_SOURCE_DIR}/src/network/server/server_tcp_transport.cpp
        # ${CMAKE_SOURCE_DIR}/include/network/server/server_tcp_transport.hpp

        ${CMAKE_SOURCE_DIR}/src/utils/json_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/json_utils.hpp
        ${CMAKE_SOURCE_DIR}/src/utils/server_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/server_utils.hpp
    )
    target_include_directories(Server
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/managers/server
            ${CMAKE_SOURCE_DIR}/include/network/server
            ${CMAKE_SOURCE_DIR}/include/utils
            ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(Server PUBLIC Qt6::Core Qt6::HttpServer)
    LIST(APPEND LIB_TARGETS Server)

    # Client lib
    qt_add_library(Client STATIC)
    target_sources(Client PRIVATE
        ${CMAKE_SOURCE_DIR}/src/managers/client/client.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/client.hpp

        ${CMAKE_SOURCE_DIR}/include/network/client/client_messenger.hpp
        ${CMAKE_SOURCE_DIR}/src/network/client/client_http_messenger.cpp
        ${CMAKE_SOURCE_DIR}/include/network/client/client_http_messenger.hpp

        ${CMAKE_SOURCE_DIR}/include/network/client/client_transport.hpp
        ${CMAKE_SOURCE_DIR}/src/network/client/client_tcp_transport.cpp
        ${CMAKE_SOURCE_DIR}/include/network/client/client_tcp_transport.hpp

        ${CMAKE_SOURCE_DIR}/src/utils/json_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/json_utils.hpp
    )
    target_include_directories(Client
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/managers/client
            ${CMAKE_SOURCE_DIR}/include/network/client
            ${CMAKE_SOURCE_DIR}/include/utils
    )
    target_link_libraries(Client PUBLIC Qt6::Core Qt6::Network ClientManagers)
    LIST(APPEND LIB_TARGETS Client)

    # Logger lib
    qt_add_library(Logger STATIC)
    target_sources(Logger PRIVATE
        ${CMAKE_SOURCE_DIR}/src/application/log.cpp
        ${CMAKE_SOURCE_DIR}/include/application/log.hpp
    )
    target_include_directories(Logger
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/application
    )
    target_link_libraries(Logger PUBLIC Qt6::Core)
    LIST(APPEND LIB_TARGETS Logger)

    foreach(LIB IN LISTS LIB_TARGETS)
        target_compile_features(${LIB} PUBLIC cxx_std_23)

        target_compile_options(${LIB} PUBLIC -Wall -Wextra -Wpedantic -Werror)

        target_compile_definitions(${LIB} PUBLIC $<$<CONFIG:Debug>:QT_MESSAGELOGCONTEXT>)
        target_compile_definitions(${LIB} PUBLIC $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>)

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
