#include "coordinator/coordinatorwindow.h"
#include "ui_coordinatorwindow.h"

#include <QMessageBox>
#include <QRegExp>

#include "utils.h"

CoordinatorWindow::CoordinatorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CoordinatorWindow),
    _socket(nullptr),
    _parser(this)
{
    ui->setupUi(this);

    ui->bt_stop->hide();

    // default text to first non localhost ipv6, fallback to ipv4
    // fallback to any
    ui->le_addr->setText(Utils::getFirstNonLocalhost(true).toString());
    // ui->le_addr->setText(QHostAddress(QHostAddress::Any).toString());
}

CoordinatorWindow::~CoordinatorWindow()
{
    delete ui;
}

void CoordinatorWindow::read_pending_datagrams()
{
    // read all pending datagrams
    while (_socket->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        // alocate mem to the new incoming datagram
        datagram->resize(_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        // read and store sender address and senderPort
        _socket->readDatagram(datagram->data(), datagram->size(),
                                &sender, &senderPort);

        // do the magic
        processDatagram(sender, senderPort, datagram);
    }
}

void  CoordinatorWindow::processDatagram(QHostAddress &senderAddr, int senderPort, QByteArray *data)
{
    QString dataStr(*data);

    QString id = senderAddr.toString()+":" + QString::number(senderPort);

    QString line = "Recv from: " + id + " " + " data: " + dataStr;
    line.remove(QRegExp("[\\n\\r]*$"));
    log( line );

    _parser.parse(dataStr);

    if(_parser.getMethods().contains(ProtocolMethod::GET_SERVER)) {
        sendServer(senderAddr, senderPort);
    }

    if(_parser.getMethods().contains(ProtocolMethod::JOIN)) {
        join(senderAddr, senderPort);
    }

    if(_parser.getMethods().contains(ProtocolMethod::SERVER_ADD)) {

        for(auto server : _parser.getServers()) {
            addServer(server, senderAddr, senderPort);
        }
    }

    if(_parser.getMethods().contains(ProtocolMethod::GET_CHANNELS)) {
        sendChannels(senderAddr, senderPort);
    }

}

void CoordinatorWindow::log(const QString &msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}

void CoordinatorWindow::on_bt_start_clicked()
{
    QHostAddress addr(ui->le_addr->text());

    // check for invalid address
    if(addr.isNull() || ui->le_addr->text().isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please inform a valid address"), QMessageBox::Ok);
        return;
    }

    // check for invalid port
    QString portStr = ui->le_port->text();
    bool ok = false;
    int port = portStr.toInt(&ok);
    if(portStr.isEmpty() || !ok) {
        QMessageBox::warning(this, tr("Error"), tr("Please inform a valid port"), QMessageBox::Ok);
        return;
    }

    log(tr("Coordinator is starting..."));

    // invert the buttons Start/stop
    ui->bt_stop->show();
    ui->bt_start->hide();

    ui->le_addr->setDisabled(true);
    ui->le_port->setDisabled(true);

    _socket = new QUdpSocket(this);

    // read slot
    connect(_socket, SIGNAL(readyRead()),
                this, SLOT(read_pending_datagrams()));

    // state changed slot
    connect(_socket,
            SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,
            SLOT(socket_changed_state(QAbstractSocket::SocketState))
            );

    // bing the socket to the requested address
    _socket->bind(addr, port);
}

void CoordinatorWindow::on_bt_stop_clicked()
{

    // close and destroy the socket
    _socket->close();
    _socket->deleteLater();
    _socket = nullptr;

    // invert the buttons Start/Stop
    ui->bt_stop->hide();
    ui->bt_start->show();

    ui->le_addr->setDisabled(false);
    ui->le_port->setDisabled(false);

}

void CoordinatorWindow::socket_changed_state(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        log(tr("Disconnected"));
        break;
    case QAbstractSocket::HostLookupState:
        log(tr("Looking for host"));
        break;
    case QAbstractSocket::ConnectingState:
        log(tr("Connecting..."));
        break;
    case QAbstractSocket::ConnectedState:
        log(tr("Connected."));
        break;
    case QAbstractSocket::BoundState:
        log(tr("Bound address"));
        break;
    case QAbstractSocket::ClosingState:
        log(tr("Closing..."));
        break;
    case QAbstractSocket::ListeningState:
        log(tr("Listening..."));
        break;
    default:
        break;
    }
}

void CoordinatorWindow::udpSend(QHostAddress &addr, int port, const QString &data)
{

    QString id = addr.toString()+":" + QString::number(port);
    QString line = "Send to: " + id + " " + " data: " + data;
    log(line);

    _socket->writeDatagram( data.toStdString().c_str(), data.length(), addr, port );

}

void CoordinatorWindow::sendServer(QHostAddress &addr, int port)
{
    if(_serverList.size()==0) return;

    // TODO: send a random server?
    ServerData *server = _serverList.values().at(qrand() % _serverList.size());

    QString data = _parser.make_SERVER_INFO(server);

    this->udpSend(addr, port, data);

}

void CoordinatorWindow::addServer(ServerData *data, QHostAddress &senderAddr, int senderPort)
{

    QString id = data->getAddress().toString() + ":" + QString::number(data->getPort());

    // if the server already exists, clear everything we know about it
    if(_serverList.contains(id)) {
        delete _serverList[id];
        for(int i=0;i<_serverTreeMap[id]->childCount();i++) {
            delete _serverTreeMap[id]->child(i);
        }
        delete _serverTreeMap[id];
    }

    _serverList[id] = data;
    _serverTreeMap[id] = new QTreeWidgetItem(ui->tree_servers);
    _serverTreeMap[id]->setText(0, id);

    if(_channelMap.contains(id)) {
        for(auto ch : _channelMap[id]) {
            delete ch;
        }
        _channelMap[id].clear();
    } else {
        _channelMap[id] = QList<ChannelData *>();
    }

    udpSend(senderAddr, senderPort, _parser.make_OK());

}

void CoordinatorWindow::updateServerListView()
{

}

void CoordinatorWindow::sendChannels(QHostAddress &addr, int port)
{
    QString data = "";
    for(auto channelLists : _channelMap.values()) {
        data += _parser.make_CHANNEL_INFO(channelLists) + "\n";
    }

    udpSend(addr, port, data);
}

void CoordinatorWindow::join(QHostAddress &addr, int port)
{
    if(_serverList.size()==0) return;

    QString channel = "";
    for(auto args : _parser.getArgs()) {
        if(args.size() < 2) continue;

        channel = args[1];

        auto it = _channelMap.begin();
        for(; it!=_channelMap.end(); it++) {
            for(auto chd : it.value()) {
                if(chd->getName() == channel) {
                    udpSend(addr, port, _parser.make_CHANNEL_INFO(chd));
                    return;
                }
            }
        }

        ServerData *server = _serverList.values().at(qrand() % _serverList.size());
        QString serverId = server->getAddress().toString()+":"+QString::number(server->getPort());
        ChannelData *newChannel = new ChannelData(channel, server->getAddress(), server->getPort());
        _channelMap[serverId].append(newChannel);

        QTreeWidgetItem *channelNode = new QTreeWidgetItem(_serverTreeMap[serverId]);
        channelNode->setText(0, channel);
        udpSend(addr, port, _parser.make_CHANNEL_INFO(newChannel));

    }
}
