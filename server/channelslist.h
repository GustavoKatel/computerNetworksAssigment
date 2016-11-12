#ifndef CHANNELSLIST_H
#define CHANNELSLIST_H

#include "server/channel.h"
#include <QtWidgets/QTabWidget>
#include <QtNetwork>

class ChannelsList : public QHash<QString, Channel*>
{
public:
    ChannelsList();

    void createChannel(const QString &channelName, QTabWidget *tabParent);
    void addUserToChannel(QTcpSocket *user, QString channelName);
};

#endif // CHANNELSLIST_H
