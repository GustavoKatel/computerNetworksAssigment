#include "client/clientwindow.h"
#include "ui_clientwindow.h"

#include <QInputDialog>
#include <QMessageBox>

#include "utils.h"

#ifdef Q_OS_UNIX

#define SYS_USERNAME qgetenv("USER")

#elif Q_OS_WIN

#define SYS_USERNAME qgetenv("USERNAME")

#endif

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow),
    _coordinatorClient(nullptr),
    _serverClient(nullptr)
{
    ui->setupUi(this);

    ui->bt_channel->setText(tr("[No channel]"));

    getNickname();

    initChat();

    initCoordinator();
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::log(const QString &msg)
{
    QString line = "<i><font color\"#dedede\">[%1]: %2</font></i>";
    ui->tb_chat->append(line.arg( tr("SYSTEM"), msg ));
}

void ClientWindow::initChat()
{
    ui->tb_chat->append("<h1>Welcome</h1>");
    ui->tb_chat->append(
                QString("<a href=\"#connectCoordinator\"> <font color=\"blue\"> %1 </font> </a>")
                .arg(tr("Click here to connect to a coordinator"))
                );
}

void ClientWindow::initCoordinator()
{


}

void ClientWindow::initServerClient(ChannelData *channel)
{
    _serverClient = new ServerClient(channel->getAddress(), channel->getPort(), this);
    _serverClient->waitForConnected();
    _serverClient->sendJoin(channel->getName());

    displayMessage("SYSTEM", "Connected to channel " + channel->getName());

    // Update GUI
    ui->bt_channel->setText(channel->getName());
    ui->list_channels->setEnabled(false);

    sendText("(connected to channel)");

    connect(_serverClient, &ServerClient::messageReceived, this,
            [this](const QString &nickname, const QString &message) {
                displayMessage(nickname, message);
            }
    );
}

void ClientWindow::destroyServerClient() {
    // Update GUI
    ui->bt_channel->setText("[No channel]");
    ui->list_channels->setEnabled(true);

    sendText("(left the channel)");

    displayMessage("SYSTEM", "Left channel");

    // Destroy connection
    _serverClient->close();
    _serverClient->deleteLater();
    _serverClient = nullptr;
}

void ClientWindow::connectToCoordinator()
{
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

            // state changed slot
            connect(_coordinatorClient,
                    SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                    this,
                    SLOT(coordinator_changed_state(QAbstractSocket::SocketState))
                    );

            connect(_coordinatorClient,
                    &CoordinatorClient::channelInfo,
                    this,
                    &ClientWindow::channel_info
                    );

            log(QString(tr("Searching for channels from: %1:%2")).arg(_coordinatorAddr.toString(), QString::number(_coordinatorPort)));

            _coordinatorClient->getChannels();
        }

        // always delete this dialog
        _connectToCoordinatorDialog->deleteLater();
        _connectToCoordinatorDialog = nullptr;

    });

    // set modal dialog to avoid user missing the window
    _connectToCoordinatorDialog->setModal(true);
    _connectToCoordinatorDialog->show();
}

void ClientWindow::coordinator_changed_state(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        log(tr("Coordinator: Disconnected"));
        ui->tb_chat->append(
                    QString("<a href=\"#connectCoordinator\"> <font color=\"blue\"> %1 </font> </a>")
                    .arg(tr("Lost connection with coordinator. Click here to try again"))
                    );
        break;
    case QAbstractSocket::HostLookupState:
        log(tr("Coordinator: Looking for host"));
        break;
    case QAbstractSocket::ConnectingState:
        log(tr("Coordinator: Connecting..."));
        break;
    case QAbstractSocket::ConnectedState:
        log(tr("Coordinator: Connected"));
        break;
    case QAbstractSocket::BoundState:
        log(tr("Coordinator: Bound address"));
        break;
    case QAbstractSocket::ClosingState:
        log(tr("Coordinator: Closing..."));
        break;
    case QAbstractSocket::ListeningState:
        log(tr("Coordinator: Listening..."));
        break;
    default:
        break;
    }
}

void ClientWindow::processCommand(const QString &line)
{

    QString parsed = line.trimmed();

    // ignore empty lines
    if(parsed.isEmpty()) {
        return;
    }

    // not a command, send it
    if(!parsed.startsWith("/")) {
        this->sendText(line);
    }

    // test commands here

}

void ClientWindow::on_le_text_returnPressed()
{
    this->processCommand(ui->le_text->text());
    ui->le_text->clear();
}

void ClientWindow::on_toolButton_clicked()
{
    this->processCommand(ui->le_text->text());
    ui->le_text->clear();
}

void ClientWindow::displayMessage(const QString &nickname, const QString &text)
{
    QString color = "red";

    if (nickname == ui->le_nickname->text()) {
        color = "blue";
    }

    ui->tb_chat->append(QString(tr("<font color=\"%1\">[%2]</font>: %3"))
                        .arg(color, nickname, text));
}

void ClientWindow::sendText(const QString &text)
{
    if (_serverClient != NULL) {
        qDebug() << "Sending message: " + text;

        _serverClient->sendMessage(ui->le_nickname->text(), text);
    }
}

void ClientWindow::on_tb_chat_anchorClicked(const QUrl &arg1)
{
    if(arg1.toString()=="#connectCoordinator") {
        connectToCoordinator();
    }
}

void ClientWindow::channel_info(QList<ChannelData *> channels)
{
    qDebug() << "channel_info";
    for(auto channel : channels) {
        QString line = "Got channel: ";

        line += channel->getName() +
                " (" +
                channel->getAddress().toString() +
                ":" +
                QString::number(channel->getPort()) +
                ")";

        log(line);

        ui->list_channels->clear();
        _knownChannels.clear();

        for(auto channel : channels) {
            _knownChannels.append(channel);
            ui->list_channels->addItem(channel->getName());
        }

    }

}


void ClientWindow::on_list_channels_itemDoubleClicked(QListWidgetItem *item)
{
    int index = ui->list_channels->row(item);
    qDebug() << _knownChannels[index]->getName();

    // Open TCP connection with server
    initServerClient(_knownChannels[index]);
}

void ClientWindow::on_bt_channel_clicked()
{
    destroyServerClient();
}

void ClientWindow::getNickname()
{

    bool ok;
    QString text = QInputDialog::getText(this, tr("ircUFPB - Digite seu nickname"),
                                         tr("Nickname:"), QLineEdit::Normal,
                                         SYS_USERNAME, &ok);
    if(!ok) {
        qApp->quit();
        return;
    }

    if(text.trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Erro"), tr("Nickname não pode ser vazio. Digite novamente"), QMessageBox::Ok);
        getNickname();
    }

    ui->le_nickname->setText(text);

}
