#include "dataclass/d_channel.h"

ChannelData::ChannelData(const QString &name) : _name(name)
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
