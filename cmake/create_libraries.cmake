function(create_libraries APP_LIB)
    set(LIB_TARGETS "")

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

    # Utils lib
    qt_add_library(Utils STATIC)
    target_sources(Utils PRIVATE
        ${CMAKE_SOURCE_DIR}/src/utils/json_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/json_utils.hpp

        ${CMAKE_SOURCE_DIR}/src/utils/manager_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/manager_utils.hpp

        ${CMAKE_SOURCE_DIR}/src/utils/client_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/client_utils.hpp

        ${CMAKE_SOURCE_DIR}/src/utils/server_utils.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/server_utils.hpp

        ${CMAKE_SOURCE_DIR}/src/utils/message_socket.cpp
        ${CMAKE_SOURCE_DIR}/include/utils/message_socket.hpp
    )
    target_include_directories(Utils
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/utils
            ${CMAKE_SOURCE_DIR}/include/managers/client
            ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(Utils PUBLIC Qt6::Core Qt6::Network Qt6::Xml)
    LIST(APPEND LIB_TARGETS Utils)

    # SelfSignedCert lib
    qt_add_library(SelfSignedCert STATIC)
    target_sources(SelfSignedCert
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src/utils/generateSslCert.cpp
            ${CMAKE_SOURCE_DIR}/include/utils/generateSslCert.hpp
    )
    target_include_directories(SelfSignedCert
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/utils
            ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(SelfSignedCert PUBLIC Qt6::Core Qt6::Network OpenSSL::SSL OpenSSL::Crypto)
    LIST(APPEND LIB_TARGETS SelfSignedCert)

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

        ${CMAKE_SOURCE_DIR}/src/managers/client/file_info.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/file_info.hpp
        ${CMAKE_SOURCE_DIR}/src/managers/client/directory_manager.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/directory_manager.hpp
    )
    target_include_directories(ClientManagers
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/managers/client
    )
    target_link_libraries(ClientManagers PUBLIC Qt6::Core Qt6::Qml Utils)
    LIST(APPEND LIB_TARGETS ClientManagers)

    # Server lib
    qt_add_library(Server STATIC)
    target_sources(Server PRIVATE
        ${CMAKE_SOURCE_DIR}/src/managers/server/server.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/server/server.hpp

        ${CMAKE_SOURCE_DIR}/src/network/server/server_websocket_messenger.cpp
        ${CMAKE_SOURCE_DIR}/include/network/server/server_websocket_messenger.hpp

        ${CMAKE_SOURCE_DIR}/src/network/server/server_tcp_tls_transport.cpp
        ${CMAKE_SOURCE_DIR}/include/network/server/server_tcp_tls_transport.hpp

        ${CMAKE_SOURCE_DIR}/src/network/server/file_sender.cpp
        ${CMAKE_SOURCE_DIR}/include/network/server/file_sender.hpp
    )
    target_include_directories(Server
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/managers/server
            ${CMAKE_SOURCE_DIR}/include/network/server
    )
    target_link_libraries(Server PUBLIC Qt6::Core Qt6::WebSockets Utils)
    LIST(APPEND LIB_TARGETS Server)

    # Client lib
    qt_add_library(Client STATIC)
    target_sources(Client PRIVATE
        ${CMAKE_SOURCE_DIR}/src/managers/client/client.cpp
        ${CMAKE_SOURCE_DIR}/include/managers/client/client.hpp

        ${CMAKE_SOURCE_DIR}/src/network/client/client_websocket_messenger.cpp
        ${CMAKE_SOURCE_DIR}/include/network/client/client_websocket_messenger.hpp

        ${CMAKE_SOURCE_DIR}/src/network/client/client_tcp_tls_transport.cpp
        ${CMAKE_SOURCE_DIR}/include/network/client/client_tcp_tls_transport.hpp

        ${CMAKE_SOURCE_DIR}/src/network/client/client_webdav.cpp
        ${CMAKE_SOURCE_DIR}/include/network/client/client_webdav.hpp
        ${CMAKE_SOURCE_DIR}/src/network/client/client_webdav_protocol.cpp
        ${CMAKE_SOURCE_DIR}/include/network/client/client_webdav_protocol.hpp
        ${CMAKE_SOURCE_DIR}/src/network/client/webdav_dir_parser.cpp
        ${CMAKE_SOURCE_DIR}/include/network/client/webdav_dir_parser.hpp
        ${CMAKE_SOURCE_DIR}/src/network/client/webdav_item.cpp
        ${CMAKE_SOURCE_DIR}/include/network/client/webdav_item.hpp
    )
    target_include_directories(Client
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/managers/client
            ${CMAKE_SOURCE_DIR}/include/network/client
    )
    target_link_libraries(Client PUBLIC Qt6::Core Qt6::WebSockets ClientManagers)
    LIST(APPEND LIB_TARGETS Client)

    # App lib
    qt_add_library(App STATIC)
    target_sources(App PRIVATE
        ${CMAKE_SOURCE_DIR}/src/application/app.cpp
        ${CMAKE_SOURCE_DIR}/include/application/app.hpp

        ${CMAKE_SOURCE_DIR}/src/application/initialize.cpp
        ${CMAKE_SOURCE_DIR}/include/application/initialize.hpp
    )
    target_include_directories(App
        PUBLIC
            ${CMAKE_SOURCE_DIR}/include/application
    )
    target_link_libraries(App PUBLIC Client Server Logger Qt6::Quick SelfSignedCert)
    LIST(APPEND LIB_TARGETS App)
    set(APP_LIB App)

    # Adding flags
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

    return(PROPAGATE APP_LIB)

endfunction()
