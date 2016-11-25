#ifndef COORDINATORSOCKET_H
#define COORDINATORSOCKET_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>

#include "dataclass/d_channel.h"
#include "dataclass/d_server.h"

#include "protocol/protocol.h"

class CoordinatorClient : public QUdpSocket
{
    Q_OBJECT
public:
    explicit CoordinatorClient(const QHostAddress &host, int port, QObject *parent = 0);

    /**
     * @brief getServer DEPRECATED
     */
    void getServer();

    void join(const QString &channelName);

    void getChannels();

    void notifyChannels(const QHostAddress &addr, int port, QList<QString> channelsName);

signals:
    void ack();

    void channelInfo(QList<ChannelData *> channels);

    void serverInfo(QList<ServerData *> servers);

public slots:

private slots:
    void read_pending_datagrams();

private:
    QHostAddress _addr;
    int _port;
    ProtocolParser _parser;

    void doDatagram(const QString &data);
    void udpSend(const QString &data);
};

#endif // COORDINATORSOCKET_H
