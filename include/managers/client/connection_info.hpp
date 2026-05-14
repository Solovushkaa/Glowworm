/**
 * @file conncetion_info.hpp
 * @brief Provides support for network connections.
 */

#ifndef CONNECTIONINFO_HPP
#define CONNECTIONINFO_HPP

#include <QLoggingCategory>
#include <QObject>
#include <QProperty>
#include <QStringView>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>

Q_DECLARE_LOGGING_CATEGORY(connection_info)

namespace connections {

/**
 * @brief All fields of information about connections.
 */
enum class ConnectionInfoMember {
    Name,
    Transport,
    Url,
    RemoteUserName,
    ConnectionState,
    SecureConnection,
    COUNT // To get the size of the enum
};

/**
 * @brief All fields of information about downloads.
 * @param member Enum
 * @return Enum type converted into QStringView
 */
QStringView getConnectionInfoMemberName(ConnectionInfoMember member);

} // namespace connections

/**
 * @class ConnectionInfo
 * @brief -
 */
class ConnectionInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Managed from C++")

public:
    enum class Transport { TCP, UDP };
    Q_ENUM(Transport)

    enum class ConnectionState {
        Disconnected, ///< No connection
        Connected,    ///< Connected
        Error         ///< Connection error
    };
    Q_ENUM(ConnectionState)

private:
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(Transport transport MEMBER m_transport NOTIFY transportChanged)
    Q_PROPERTY(QUrl url MEMBER m_url NOTIFY urlChanged)
    Q_PROPERTY(QString remoteUserName MEMBER m_remoteUserName NOTIFY remoteUserNameChanged)
    Q_PROPERTY(ConnectionState connectionState MEMBER m_connectionState NOTIFY connectionStateChanged)

    // --- Constructors ---
public:
    explicit ConnectionInfo(QObject *parent = nullptr);
    ConnectionInfo(const QString &name,
                   Transport transport,
                   const QUrl &url,
                   const QString &remoteUserName,
                   ConnectionState connectionState,
                   bool isSecureConnection,
                   QObject *parent = nullptr);

    ConnectionInfo(const ConnectionInfo &connectionInfo);
    ConnectionInfo &operator=(const ConnectionInfo &connectionInfo);

    ConnectionInfo(ConnectionInfo &&connectionInfo);
    ConnectionInfo &operator=(ConnectionInfo &&connectionInfo);

    ~ConnectionInfo();

    // --- Set Methods ---
public:
    void setConnectionState(ConnectionState state)
    {
        m_connectionState = state;
        emit connectionStateChanged();
    }

    // --- Signals ---
signals:
    void nameChanged();
    void transportChanged();
    void urlChanged();
    void remoteUserNameChanged();
    void connectionStateChanged();

public:
    // --- General ---
    QString m_name;        ///< Connection name
    Transport m_transport; ///< Network transport

    // --- Network ---
    QUrl m_url; ///< url
    QString m_remoteUserName; ///< Remote user (not needed for LAN)

    ConnectionState m_connectionState; ///< Connection state

    bool m_isSecureConnection;
};

#endif // CONNECTIONINFO_HPP
