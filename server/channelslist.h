#ifndef CHANNELSLIST_H
#define CHANNELSLIST_H

#include "server/channel.h"
#include <QtWidgets/QTabWidget>

class ChannelsList : public QHash<QString, Channel*>
{
public:
    ChannelsList();

    void createChannel(const QString &channelName, QTabWidget *tabParent);
};

#endif // CHANNELSLIST_H
