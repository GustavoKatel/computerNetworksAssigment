#include "dataclass/d_channel.h"

ChannelData::ChannelData(const QString &name, const QHostAddress &host, int port) :
    _name(name),
    _addr(host),
    _port(port)
{

}

const QString &ChannelData::getName() const
{
    return this->_name;
}

void ChannelData::setName(const QString &name)
{
    this->_name = name;
}

const QHostAddress &ChannelData::getAddress() const
{
    return this->_addr;
}

void ChannelData::setAddress(const QHostAddress &host)
{
    this->_addr = host;
}

int ChannelData::getPort() const
{
    return this->_port;
}

void ChannelData::setPort(int port)
{
    this->_port = port;
}
