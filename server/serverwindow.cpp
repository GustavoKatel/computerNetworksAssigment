#include "server/serverwindow.h"
#include "ui_serverwindow.h"

#include <QDateTime>
#include <QtWidgets>
#include <QtNetwork>

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow), 
    _parser(this),
    _coordinatorClient(nullptr),
    _notifyChannelsTimer(nullptr)
{
    startServer();

    connectToCoordinator();
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

// TODO: Code almost duplicated from Client
void ServerWindow::connectToCoordinator()
{
    // hide main window
    this->hide();

    // always create a dialog, since it deletes itself when closed. See below
    _connectToCoordinatorDialog = new ConnectToCoordinatorDialog(this);

    // connects the finished slot and retrieves the inputted information
    connect(_connectToCoordinatorDialog, &QDialog::finished, this, [this](int result){

        if(result) {
            _coordinatorAddr = QHostAddress(_connectToCoordinatorDialog->getAddress());
            _coordinatorPort = _connectToCoordinatorDialog->getPort();

            // memory safety first :p
            if(_coordinatorClient) {
                _coordinatorClient->close();
                _coordinatorClient->deleteLater();
                _coordinatorClient = nullptr;
            }

            _coordinatorClient = new CoordinatorClient(_coordinatorAddr, _coordinatorPort, this);

            // TODO: Wait for OK for x seconds and close everything if no answer
            _coordinatorClient->serverAdd(tcpServer->serverAddress(), tcpServer->serverPort());

            // Register handler to notify coordinator of channels every X seconds
            _notifyChannelsTimer = new QTimer(this);
            connect(_notifyChannelsTimer, &QTimer::timeout, this, &ServerWindow::notifyCurrentChannels);
            _notifyChannelsTimer->start(3000); //time specified in ms
        }

        // always delete this dialog
        _connectToCoordinatorDialog->deleteLater();
        _connectToCoordinatorDialog = nullptr;

    });

    // set modal dialog to avoid user missing the window
    _connectToCoordinatorDialog->setModal(true);
    _connectToCoordinatorDialog->show();
}

// Create default channel and start TCP server
void ServerWindow::startServer()
{
    ui->setupUi(this);

    // Create channels list
    channelsList = new ChannelsList();

    createChannel("default-channel");

    log("Starting server");
    startTCPServer();
    log("Server started");
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

// Parse messages from users
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

// Called once a new user connects
// Insert new user on default channel and register on_readyRead
// So we can parse all his messages
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

// Notify coordinator of current channels
void ServerWindow::notifyCurrentChannels() {
    // log("Notifying coordinator of channels");

    _coordinatorClient->notifyChannels(channelsList->keys());
}
