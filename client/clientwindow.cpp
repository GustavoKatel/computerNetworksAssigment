#include "client/clientwindow.h"
#include "ui_clientwindow.h"

#include "utils.h"

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow),
    _coordinatorClient(nullptr)
{
    ui->setupUi(this);

    ui->bt_channel->setText(tr("[No channel]"));

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

            log(QString(tr("Quering server from: %1:%2")).arg(_coordinatorAddr.toString(), QString::number(_coordinatorPort)));

            _coordinatorClient->getServer();
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

void ClientWindow::sendText(const QString &text)
{
    ui->tb_chat->append(QString(tr("<font color=\"red\">[%1]</font>: %2"))
                        .arg(ui->le_nickname->text(), text));
}

void ClientWindow::on_tb_chat_anchorClicked(const QUrl &arg1)
{
    if(arg1.toString()=="#connectCoordinator") {
        connectToCoordinator();
    }
}
