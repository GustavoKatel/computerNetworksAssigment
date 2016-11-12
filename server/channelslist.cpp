#include "channelslist.h"

ChannelsList::ChannelsList()
{

}

void ChannelsList::createChannel(const QString &channelName, QTabWidget *tabParent)
{
    // Create channel
    // TODO: Check if it already exists
    Channel *channel = new Channel(channelName, tabParent);
    insert(channelName, channel);
}

void ChannelsList::addUserToChannel(QTcpSocket *user, QString channelName)
{
    Channel *channel = value(channelName);

    // TODO: Check if channels exist

    channel->addUser(user);
}
