#include "serverclient.h"

ServerClient::ServerClient(const QHostAddress &host, int port, QObject *parent) :
    QTcpSocket(parent),
    _addr(host),
    _port(port),
    _parser(this)
{

}
