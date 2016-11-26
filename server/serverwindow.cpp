#include "server/serverwindow.h"
#include "ui_serverwindow.h"

#include <QDateTime>
#include <QtWidgets>
#include <QtNetwork>

#include <utils.h>

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow), 
    _parser(this),
    _coordinatorClient(nullptr),
    _notifyChannelsTimer(nullptr),
    tcpServer(nullptr)
{
    ui->setupUi(this);
    initializeTextFields();

    connectToCoordinator();
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

// TODO: Code almost duplicated from Client
void ServerWindow::connectToCoordinator()
{
    // always create a dialog, since it deletes itself when closed. See below
    _connectToCoordinatorDialog = new ConnectToCoordinatorDialog(this);

    // connects the finished slot and retrieves the inputted information
    connect(_connectToCoordinatorDialog, &QDialog::finished, this, [this](int result){

        if(result) {
            _coordinatorAddr = QHostAddress(_connectToCoordinatorDialog->getAddress());
            _coordinatorPort = _connectToCoordinatorDialog->getPort();

            _coordinatorClient = new CoordinatorClient(_coordinatorAddr, _coordinatorPort, this);

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

    QHostAddress hostAddress = QHostAddress(ui->textEditIP->toPlainText());
    int port = ui->textEditPort->toPlainText().toInt();

    if (!tcpServer->listen(hostAddress, port)) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }

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

    if (tcpServer != NULL) {
        _coordinatorClient->notifyChannels(tcpServer->serverAddress(), tcpServer->serverPort(), channelsList->keys());
    }
}

void ServerWindow::on_btnStartServer_clicked()
{
    startServer();

    this->ui->btnStartServer->setEnabled(false);
    this->ui->textEditPort->setEnabled(false);

    this->ui->textEditChannelName->setEnabled(true);
    this->ui->btnCreateChannel->setEnabled(true);
}

void ServerWindow::initializeTextFields()
{
    QHostAddress hostAddress = Utils::getFirstNonLocalhost(true);
    int port = 1234;

    // Update fields with current IP and Port
    ui->textEditIP->setText(hostAddress.toString());
    ui->textEditPort->setText(QString::number(port));
}

void ServerWindow::on_btnCreateChannel_clicked()
{
    QString channelName = ui->textEditChannelName->toPlainText().replace(" ", "-");
    ui->textEditChannelName->clear();

    createChannel(channelName);
}
