#include "dataclass//d_server.h"

ServerData::ServerData(const QHostAddress &addr, int port, int totalClients) :
    _addr{addr}, _port{port}, _totalClients{totalClients}
{

}

const QHostAddress &ServerData::getAddress() const
{
    return this->_addr;
}

int ServerData::getPort() const
{
    return this->_port;
}

int ServerData::getTotalClients() const
{
    return this->_totalClients;
}

int ServerData::incClients()
{
    return ++this->_totalClients;
}

int ServerData::decClients()
{
    return --this->_totalClients;
}
