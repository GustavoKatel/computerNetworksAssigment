#include "coordinator/coordinatorwindow.h"
#include "ui_coordinatorwindow.h"

#include <QMessageBox>
#include <QRegExp>

#include "utils.h"

#define CT_REGEX_GET_SERVER "^GET SERVER$"

CoordinatorWindow::CoordinatorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CoordinatorWindow),
    _socket(nullptr)
{
    ui->setupUi(this);

    ui->bt_stop->hide();

    // default text to "0.0.0.0", if IPv4
    QHostAddress anyAddr(QHostAddress::Any);
    ui->le_addr->setText(anyAddr.toString());
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
    // removes eol
    dataStr.remove(QRegExp("[\\n\\r]*$"));

    QString id = senderAddr.toString()+":" + QString::number(senderPort);

    QString line = "Recv from: " + id + " " + " data: " + dataStr;
    log( line );

    // test regex
    if(Utils::testRegex(CT_REGEX_GET_SERVER, dataStr)) {
        this->sendServer(senderAddr, senderPort);
    }

}

void CoordinatorWindow::log(QString msg)
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

void CoordinatorWindow::udpSend(QHostAddress &addr, int port, QString &data)
{

    QString id = addr.toString()+":" + QString::number(port);
    QString line = "Send to: " + id + " " + " data: " + data;
    log(line);

    _socket->writeDatagram( data.toStdString().c_str(), data.length(), addr, port );

}

void CoordinatorWindow::sendServer(QHostAddress &addr, int port)
{
    // TODO: send a random server?
//    Server *server = _serverList.begin().value();
//    QString data = "SERVER " + server->getAddress().toString() + " " + server->getPort();

    QString data = "SERVER ";

    this->udpSend(addr, port, data);

}
