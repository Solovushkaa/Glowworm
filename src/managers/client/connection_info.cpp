#include "connection_info.hpp"
#include "constants.hpp"
#include <stdexcept>
Q_LOGGING_CATEGORY(connection_info, "connection.info")

namespace connections {

QStringView getConnectionInfoMemberName(ConnectionInfoMember member)
{
    using enum ConnectionInfoMember;

    switch (member) {
    case Name:
        return constants::kName;
    case Transport:
        return constants::kTransport;
    case Url:
        return constants::kURL;
    case RemoteUserName:
        return constants::kRemoteUserName;
    case ConnectionState:
        return constants::kConnectionState;
    case SecureConnection:
        return constants::kIsSecureConnection;
    default:
        throw std::invalid_argument("Unknown ConnectionInfoMember to convert to string");
    }
}

} // namespace connections

ConnectionInfo::ConnectionInfo(QObject *parent)
    : QObject(parent)
{
    qCDebug(connection_info) << "ConnectionInfo - created";
}

ConnectionInfo::ConnectionInfo(const QString &name,
                               Transport transport,
                               const QUrl &url,
                               const QString &remoteUserName,
                               ConnectionState connectionState,
                               bool isSecureConnection,
                               QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_transport(transport)
    , m_url(url)
    , m_remoteUserName(remoteUserName)
    , m_connectionState(connectionState)
    , m_isSecureConnection(isSecureConnection)
{
    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- created";
}

ConnectionInfo::ConnectionInfo(const ConnectionInfo &connectionInfo)
{
    *this = connectionInfo;
}

ConnectionInfo &ConnectionInfo::operator=(const ConnectionInfo &connectionInfo)
{
    m_name = connectionInfo.m_name;
    m_transport = connectionInfo.m_transport;
    m_url = connectionInfo.m_url;
    m_remoteUserName = connectionInfo.m_remoteUserName;
    m_connectionState = connectionInfo.m_connectionState;
    m_isSecureConnection = connectionInfo.m_isSecureConnection;
    this->setParent(connectionInfo.parent());

    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- copyed";

    return *this;
}

ConnectionInfo::ConnectionInfo(ConnectionInfo &&connectionInfo)
{
    *this = std::move(connectionInfo);
}

ConnectionInfo &ConnectionInfo::operator=(ConnectionInfo &&connectionInfo)
{
    m_name = std::move(connectionInfo.m_name);
    m_transport = connectionInfo.m_transport;
    m_url = std::move(connectionInfo.m_url);
    m_remoteUserName = std::move(connectionInfo.m_remoteUserName);
    m_connectionState = connectionInfo.m_connectionState;
    m_isSecureConnection = connectionInfo.m_isSecureConnection;
    this->setParent(connectionInfo.parent());

    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- moved";

    return *this;
}

ConnectionInfo::~ConnectionInfo()
{
    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- destroyed";
}
