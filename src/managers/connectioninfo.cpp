#include "connectioninfo.h"

ConnectionInfo::ConnectionInfo(QObject *parent)
    : QObject{parent}
{}

ConnectionInfo &ConnectionInfo::operator=(const ConnectionInfo &connInfo)
{
    m_url = connInfo.m_url;
    m_name = connInfo.m_name;
    m_protocol = connInfo.m_protocol;
    m_ip = connInfo.m_ip;
    m_port = connInfo.m_port;

    return *this;
}

ConnectionInfo &ConnectionInfo::operator=(ConnectionInfo &&connInfo)
{
    m_url = std::move(connInfo.m_url);
    m_name = std::move(connInfo.m_name);
    m_protocol = std::move(connInfo.m_protocol);
    m_ip = std::move(connInfo.m_ip);
    m_port = std::move(connInfo.m_port);

    return *this;
}
