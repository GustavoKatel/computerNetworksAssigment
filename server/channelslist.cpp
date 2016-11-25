#include "channelslist.h"

ChannelsList::ChannelsList() :
    QHash<QString, Channel*>()
{

}

void ChannelsList::createChannel(const QString &channelName, QTabWidget *tabParent)
{
    // Create channel
    // TODO: Check if it already exists
    ChannelData *channel = new ChannelData(channelName, tabParent);
    insert(channelName, channel);
}

void ChannelsList::addUserToChannel(QTcpSocket *user, QString channelName)
{
    ChannelData *channel = value(channelName);

    // TODO: Check if channels exist

    channel->addUser(user);
}
