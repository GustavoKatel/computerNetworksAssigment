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
