#include "client/clientwindow.h"
#include "ui_clientwindow.h"

#include "utils.h"

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow),
    _coordinatorSocket(nullptr)
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
    // memory safety first :p
    if(_coordinatorSocket) {
        _coordinatorSocket->close();
        _coordinatorSocket->deleteLater();
        _coordinatorSocket = nullptr;
    }
    _coordinatorSocket = new QUdpSocket(this);
    // read slot
    connect(_coordinatorSocket, SIGNAL(readyRead()),
                this, SLOT(coordinator_read_pending_datagrams()));

    // state changed slot
    connect(_coordinatorSocket,
            SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,
            SLOT(coordinator_changed_state(QAbstractSocket::SocketState))
            );

    _coordinatorSocket->bind(0);
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
            log(QString(tr("Quering server from: %1:%2")).arg(_coordinatorAddr.toString(), QString::number(_coordinatorPort)));

            QString query("GET SERVER");
            _coordinatorSocket->writeDatagram(query.toStdString().c_str(), query.length(), _coordinatorAddr, _coordinatorPort);
        }

        // always delete this dialog
        _connectToCoordinatorDialog->deleteLater();
        _connectToCoordinatorDialog = nullptr;

    });

    // set modal dialog to avoid user missing the window
    _connectToCoordinatorDialog->setModal(true);
    _connectToCoordinatorDialog->show();
}

void ClientWindow::coordinator_read_pending_datagrams()
{
    // read all pending datagrams
    while (_coordinatorSocket->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        // alocate mem to the new incoming datagram
        datagram->resize(_coordinatorSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        // read and store sender address and senderPort
        _coordinatorSocket->readDatagram(datagram->data(), datagram->size(),
                                &sender, &senderPort);

        QString dataStr(*datagram);
        dataStr.remove(QRegExp("[\\n\\r]*$"));
        log("Coordinator says: " + dataStr);
    }
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
