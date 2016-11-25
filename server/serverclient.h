#ifndef SERVERCLIENT_H
#define SERVERCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>

#include <protocol/protocol.h>

class ServerClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ServerClient(const QHostAddress &host, int port, QObject *parent = 0);

    void sendMessage(const QString &message);

signals:
    void messageReceived(const QString &message);

private slots:
    void on_readyRead();

private:
    QHostAddress _addr;
    int _port;

    QDataStream in;

    ProtocolParser _parser;
};

#endif // SERVERCLIENT_H
