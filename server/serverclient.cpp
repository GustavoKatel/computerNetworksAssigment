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

void ServerClient::sendMessage(const QString &message)
{
    qDebug() << "sending message " + message;

    this->write(message.toUtf8()+"\n");
    this->waitForBytesWritten();
}

void ServerClient::on_readyRead()
{
    if(this->canReadLine())
    {
        QByteArray byteArray = this->readLine();

        emit messageReceived(byteArray);
    }
}
