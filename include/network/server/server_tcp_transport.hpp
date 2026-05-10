#ifndef SERVERTCPTRANSPORT_HPP
#define SERVERTCPTRANSPORT_HPP

#include "server_transport.hpp"

class ServerTcpTransport : public ServerTransport
{
    Q_OBJECT

public:
    explicit ServerTcpTransport(QObject *parent = nullptr);
};

#endif // SERVERTCPTRANSPORT_HPP
