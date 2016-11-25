#include "serverclient.h"

ServerClient::ServerClient(const QHostAddress &host, int port, QObject *parent) :
    QTcpSocket(parent),
    _addr(host),
    _port(port),
    _parser(this)
{
    this->connectToHost(host, port);
    this->waitForConnected(3000);

    if(this->state() == this->ConnectedState) {
        qDebug() << "Connected to server!";
    }

    connect(this, &QIODevice::readyRead,
            this, &ServerClient::on_readyRead);
}

void ServerClient::sendMessage(const QString nickname, const QString &message)
{
    QString formattedMessage = nickname + "|" + message + "\n";

    this->write(formattedMessage.toUtf8());
    this->waitForBytesWritten();
}

void ServerClient::on_readyRead()
{
    if(this->canReadLine())
    {
        QString fullMessage = this->readLine();

        QString nickname;
        QString message;

        if (fullMessage.contains("|")) {
            // If message contains (it should) the nickname of the sender
            nickname = fullMessage.split("|")[0];
            message = fullMessage.split("|")[1];
        } else {
            // Otherwise, handle it properly (useful for when debugging with nc)
            nickname = "SYSTEM";
            message = fullMessage;
        }

        emit messageReceived(nickname, message);
    }
}
