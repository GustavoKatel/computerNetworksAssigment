#include "coordinator/server.h"

Server::Server(QHostAddress &addr, int port, int totalClients) :
    _addr{addr}, _port{port}, _totalClients{totalClients}
{

}

const QHostAddress &Server::getAddress() const
{
    return this->_addr;
}

int Server::getPort() const
{
    return this->_port;
}

int Server::getTotalClients() const
{
    return this->_totalClients;
}

int Server::incClients()
{
    return ++this->_totalClients;
}

int Server::decClients()
{
    return --this->_totalClients;
}
