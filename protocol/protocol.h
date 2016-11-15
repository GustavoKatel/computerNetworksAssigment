#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QStringList>
#include <QList>

#include "dataclass/d_server.h"
#include "dataclass/d_channel.h"

enum ProtocolMethod {
    GET_SERVER = 0, // client->coord
    JOIN, // client->coord
    SERVER_ADD, // server->coord
    SERVER_INFO, // coord->client (response to QUERY_SERVER)
    GET_CHANNELS, // client->coord
    CHANNEL_INFO, // coord->client
    OK, // GENERAL ACK
    NO_METHOD
};

class ProtocolParser : public QObject {

    Q_OBJECT

public:
    explicit ProtocolParser(QObject *parent=0);

    bool parse(const QString &data);

    const QList<ProtocolMethod> &getMethods() const;

    const QList<QStringList> &getArgs() const;

    /**
     * @brief getChannels Get all the channels in the batch
     * PS.: Delete the objects yourself
     * @return QList<ChannelData *> * Pointer to list of ChannelData pointers
     */
    const QList<ChannelData *> &getChannels();

    /**
     * @brief getServers Get all the servers in the batch
     * PS.: Delete the objects yourself
     * @return QList<ServerData *> * Pointer to list of ServerData pointers
     */
    const QList<ServerData *> &getServers();

    /**
     * @brief make Creates a protocol message
     * @param method ProtocolMethod The protocol method
     * @param args QStringList A list of args
     * @return QString containing the protocol message
     */
    QString make(const ProtocolMethod &method, const QStringList &args);

    /**
     * @brief makeBatch Creates a batch of protocol messages
     * @param methods A lost of ProtocolMethod's
     * @param argList A list of args of each ProtocolMethod in @methods
     * @return QString containing the created batch
     */
    QString makeBatch(const QList<ProtocolMethod> &methods, const QList<QStringList> &argList);

    QString make_CHANNEL_INFO(ChannelData * channel);
    QString make_CHANNEL_INFO(const QList<ChannelData *> &channels);

    QString make_GET_SERVER();

    QString make_JOIN(const QString &channelName);

    QString make_SERVER_ADD(const QHostAddress &addr, int port);

    QString make_GET_CHANNELS();

    QString make_OK();

private:
    QString _data;

    QList<ProtocolMethod> _methods;

    QList<QStringList> _args;

    QList<ServerData *> _servers;
    QList<ChannelData *> _channels;

    bool parseLine(const QString &data);

};

#endif // PROTOCOL_H
