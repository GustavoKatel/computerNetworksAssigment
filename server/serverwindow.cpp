#include "server/serverwindow.h"
#include "ui_serverwindow.h"

#include <QDateTime>
#include <QtWidgets>

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    startTCPServer();
    log("Starting server");
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
    log(tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                            "Run the Fortune Client example now.")
                         .arg(ipAddress).arg(tcpServer->serverPort()));

    connect(tcpServer, &QTcpServer::newConnection, this,
            &ServerWindow::handleConnection);
}

void ServerWindow::log(QString message)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    ui->textEditLog->appendPlainText(
                currentTime.toString() + ": " + message);
}

void ServerWindow::echo(QTcpSocket *tcpSocket)
{
    if(tcpSocket->canReadLine())
    {
        QByteArray ba = tcpSocket->readLine();

        log(QString(ba));

        tcpSocket->write(ba);
    }
}

void ServerWindow::handleConnection() {
    while (tcpServer->hasPendingConnections()) {
        log("New connection!");

        QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
        connect(tcpSocket, &QIODevice::readyRead,
                this, [this, tcpSocket]{ echo(tcpSocket); });
    }
}
