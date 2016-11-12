#include "server/serverwindow.h"
#include "server/channel.h"
#include "ui_serverwindow.h"

#include <QDateTime>
#include <QtWidgets>

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    createChannel();

    log("Starting server");
    startTCPServer();
    log("Server started");
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::startTCPServer()
{
    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, 1234)) {
        QMessageBox::critical(this, tr("Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    // Update fields with current IP and Port
    ui->textEditIP->setText(ipAddress);
    ui->textEditPort->setText(QString::number(tcpServer->serverPort()));

    connect(tcpServer, &QTcpServer::newConnection, this,
            &ServerWindow::handleConnection);
}

void ServerWindow::log(QString message)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    ui->textEditLog->appendPlainText(
                currentTime.toString() + ": " + message);
}

void ServerWindow::on_readyRead(QTcpSocket *tcpSocket)
{
    if(tcpSocket->canReadLine())
    {
        QByteArray ba = tcpSocket->readLine();

        // Parse message
        createChannel();
        log(QString(ba));

        tcpSocket->write(ba);
    }
}

void ServerWindow::handleConnection() {
    while (tcpServer->hasPendingConnections()) {
        log("New connection!");

        QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
        connect(tcpSocket, &QIODevice::readyRead,
                this, [this, tcpSocket]{ on_readyRead(tcpSocket); });
    }
}

void ServerWindow::createChannel() {
    // Create channel
    // Insert tab

    Channel *channel = new Channel("lol", ui->tabWidget);
}
