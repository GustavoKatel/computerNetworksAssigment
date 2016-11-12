#include "server/serverwindow.h"
#include "ui_serverwindow.h"

#include <QDateTime>
#include <QtWidgets>

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    // Create channels list
    channelsList = new ChannelsList();

    createChannel("default-channel");

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
                currentTime.toString() + ": " + message.trimmed());
}

void ServerWindow::on_readyRead(QTcpSocket *tcpSocket)
{
    if(tcpSocket->canReadLine())
    {
        QByteArray ba = tcpSocket->readLine();

        // Parse message
        // If create channel
        createChannel(QString(ba).trimmed());

        // If send message to a channel

        log(QString(ba));

        // Write back to user
        // tcpSocket->write(ba);
    }
}

void ServerWindow::handleConnection() {
    while (tcpServer->hasPendingConnections()) {
        log("New connection!");

        QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();

        // Add user to channel
        addUserToChannel(tcpSocket, "default-channel");

        connect(tcpSocket, &QIODevice::readyRead,
                this, [this, tcpSocket]{ on_readyRead(tcpSocket); });
    }
}

void ServerWindow::createChannel(QString channelName) {
    // Add to channel data structure
    channelsList->createChannel(channelName, ui->tabWidget);

    log(tr("Number of channels: %1.").arg(channelsList->size()));
}

void ServerWindow::addUserToChannel(QTcpSocket *user, QString channelName)
{
    channelsList->addUserToChannel(user, channelName);
}

