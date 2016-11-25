#ifndef SERVERCLIENT_H
#define SERVERCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

#include <protocol/protocol.h>

class ServerClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ServerClient(const QHostAddress &host, int port, QObject *parent = 0);

    void sendMessage(const QString &message);

signals:
    void messageReceived(const QString &message);

private:
    QHostAddress _addr;
    int _port;
    ProtocolParser _parser;

    void tcpSend(const QString &data);
};

#endif // SERVERCLIENT_H
