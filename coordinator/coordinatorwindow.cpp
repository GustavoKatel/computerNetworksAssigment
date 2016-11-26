#include "coordinator/coordinatorwindow.h"
#include "ui_coordinatorwindow.h"

#include <QMessageBox>
#include <QRegExp>
#include <QDateTime>

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

    initTimer();
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

    if(_parser.getMethods().contains(ProtocolMethod::GET_CHANNELS)) {
        sendChannels(senderAddr, senderPort);
    }

    if(_parser.getMethods().contains(ProtocolMethod::NOTIFY_CHANNELS)) {
        registerChannels(senderAddr, senderPort);
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

void CoordinatorWindow::registerChannels(QHostAddress &senderAddr, int senderPort)
{

    QString id, host, port;
    for(auto argList : _parser.getArgs()) {

        argList.removeFirst(); // remove line (first capture group)

        host = argList[0];
        port = argList[1];
        id = host + ":" + port;

        QStringList channels = argList[2].split(" ");

        // if the server already exists, clear everything we know about it
        if(_serverList.contains(id)) {
            for(int i=0;i<_serverTreeMap[id]->childCount();i++) {
                qDeleteAll(_serverTreeMap[id]->takeChildren());
            }
        } else {
            _serverList[id] = new ServerData(QHostAddress(host), port.toInt());
            _serverTreeMap[id] = new QTreeWidgetItem(ui->tree_servers);
            _serverTreeMap[id]->setText(0, id);
        }

        if(_channelMap.contains(id)) {
            for(auto chd : _channelMap[id]) {
                delete chd;
            }
            _channelMap[id].clear();
        } else {
            _channelMap[id] = QList<ChannelData *>();
        }

        for(auto channelName : channels) {
            _channelMap[id].append(new ChannelData(channelName, QHostAddress(host), port.toInt()));
            QTreeWidgetItem *channelNode = new QTreeWidgetItem(_serverTreeMap[id]);
            channelNode->setText(0, channelName);
        }

        _serverPingMap[id] =  QDateTime::currentMSecsSinceEpoch();

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

void CoordinatorWindow::initTimer()
{
    _cleanDeadServersTimer.setInterval(1000);

    connect(&_cleanDeadServersTimer, &QTimer::timeout, this, &CoordinatorWindow::timer_timeout);

    _cleanDeadServersTimer.start();
}

void CoordinatorWindow::timer_timeout()
{

    long lastPing, currentMsec = QDateTime::currentMSecsSinceEpoch();

    for(auto serverId : _serverList.keys()) {

        lastPing = _serverPingMap[serverId];
        if(currentMsec - lastPing > 5000) {

            for(int i=0;i<_serverTreeMap[serverId]->childCount();i++) {
                qDeleteAll(_serverTreeMap[serverId]->takeChildren());
            }

            delete _serverTreeMap[serverId];

            for(auto chd : _channelMap[serverId]) {
                delete chd;
            }
            _channelMap[serverId].clear();

            _serverList.remove(serverId);
            _serverTreeMap.remove(serverId);
            _channelMap.remove(serverId);
            _serverPingMap.remove(serverId);

        }

    }

}
