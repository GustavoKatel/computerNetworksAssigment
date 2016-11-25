#include "coordinatorclient.h"

CoordinatorClient::CoordinatorClient(const QHostAddress &host, int port, QObject *parent) :
    QUdpSocket(parent),
    _addr(host),
    _port(port),
    _parser(this)
{

    connect(this, SIGNAL(readyRead()),
                this, SLOT(read_pending_datagrams()));

    this->bind(0);

}

void CoordinatorClient::read_pending_datagrams()
{

    // read all pending datagrams
    while (this->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        // alocate mem to the new incoming datagram
        datagram->resize(this->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        // read and store sender address and senderPort
        this->readDatagram(datagram->data(), datagram->size(),
                                &sender, &senderPort);

        QString dataStr(*datagram);

        doDatagram(dataStr);
    }

}


void CoordinatorClient::doDatagram(const QString &data)
{
    bool res = _parser.parse(data);

    if(_parser.getMethods().contains(ProtocolMethod::SERVER_INFO)) {
        emit serverInfo(_parser.getServers());
    }

    if(_parser.getMethods().contains(ProtocolMethod::CHANNEL_INFO)) {
        emit channelInfo(_parser.getChannels());
    }

    if(_parser.getMethods().contains(ProtocolMethod::OK)) {
        emit ack();
    }

}

void CoordinatorClient::join(const QString &channelName)
{
    udpSend(_parser.make_JOIN(channelName));
}

void CoordinatorClient::getChannels()
{
    udpSend(_parser.make_GET_CHANNELS());
}

void CoordinatorClient::notifyChannels(const QHostAddress &addr, int port, QList<QString> channelsName)
{
    udpSend(_parser.make_NOTIFY_CHANNELS(addr, port, channelsName));
}

void CoordinatorClient::udpSend(const QString &data)
{
    this->writeDatagram(data.toStdString().c_str(), _addr, _port);
    qDebug() << "Sending: " << data;
}
