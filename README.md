# Glowworm

Glowworm is a cross-platform desktop application for network data exchange. Built on the basis of Qt and QML, it provides a modern graphical interface for file sharing, synchronization, and remote storage management using the WebDAV protocol.

✨ Features:
- Network data exchange – seamless file transfer between users over the network
- WebDAV support – connect to WebDAV servers for remote file access and synchronization
- Modern QML interface – responsive and visually appealing user interface
- Integration with the relay server – ensures communication even in complex network environments
- Completely secure data transfer when activating session keys

🛠️ Technology Stack:
- Language: C++20
- Framework: Qt
- UI: QML / Qt Quick
- Build System: CMake
- Protocols: Websocket, WebDAV, custom network protocols
- Platforms: Windows, Linux (Support for Android, MacOS, IOS will be added in the future)

# Preview

![image](https://github.com/user-attachments/assets/bcefd4c9-0ba4-43c0-8fa8-c1349f8b7f73)

# Documentation

User guide: <a href="DOC.md">Documentation</a>

# Build

CMake is used for the build. 

Dependencies:
 - CMake: 3.25 or later
 - Ninja Multi-Config: 1.11.1 or later
 - Qt: 6.8 or later
 - Qt Modules: Core, Quick, Qml, Test, Network, WebSockets, Xml
 - C++: 20 or later
 - OpenSSL

To build, you need to create <b>CMakeUserPresets.json</b> in the root of with the following content:

~~~json
{
  "version": 6,
  "configurePresets": [
    {
      "name": "multi-config",
      "inherits": "multi-config-base",
      "cacheVariables": {
        "Qt6_DIR": <Path-to-Qt6-cmake-scripts>,
        "CMAKE_INSTALL_PREFIX": <Path-to-install-application>
      }
    }
  ]
}

~~~

<b>All subsequent actions are performed from the project root.</b>

Next, you need to run the project configuration:

~~~sh
cmake --preset multi-config
~~~

There are 2 build options available, Debug and Release.

For an assembly with additional debugging output, the Debug assembly is used.:

~~~sh
cmake --build --preset debug
~~~

For the release build, it is used:

~~~sh
cmake --build --preset release
~~~

The final executable file is on the way:
~~~sh
"./build/<Debug/Release>/Glowworm"
~~~