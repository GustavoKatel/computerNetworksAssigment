#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QStringList>
#include <QList>

enum ProtocolMethod {
    QUERY_SERVER = 0, // client->coord
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

    const ProtocolMethod &getMethod() const;

    const QStringList &getArgs() const;

    const QStringList &getChannels() const;

    QString make(const ProtocolMethod &method, const QStringList &args);

private:
    QString _data;

    ProtocolMethod _currentMethod;

    QStringList _args;

    QStringList _channels;

    bool parseLine(const QString &data);

};

#endif // PROTOCOL_H
