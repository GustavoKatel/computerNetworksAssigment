#include "client/clientwindow.h"
#include "ui_clientwindow.h"

#include "utils.h"

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow),
    connectToCoordinatorDialog(this)
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
    _socketCoordinator = new QUdpSocket(this);
    // read slot
    connect(_socketCoordinator, SIGNAL(readyRead()),
                this, SLOT(coordinator_read_pending_datagrams()));

    // state changed slot
    connect(_socketCoordinator,
            SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,
            SLOT(coordinator_changed_state(QAbstractSocket::SocketState))
            );

    _socketCoordinator->bind(0);
}

void ClientWindow::connectToCoordinator()
{
    connectToCoordinatorDialog.setModal(true);
    connectToCoordinatorDialog.show();
}

void ClientWindow::coordinator_read_pending_datagrams()
{
    // read all pending datagrams
    while (_socketCoordinator->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        // alocate mem to the new incoming datagram
        datagram->resize(_socketCoordinator->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        // read and store sender address and senderPort
        _socketCoordinator->readDatagram(datagram->data(), datagram->size(),
                                &sender, &senderPort);

        QString dataStr(*datagram);
        dataStr.remove(QRegExp("[\\n\\r]*$"));
        log(dataStr);
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
}

void ClientWindow::on_toolButton_clicked()
{
    this->processCommand(ui->le_text->text());
}

void ClientWindow::sendText(const QString &text)
{
    ui->tb_chat->append("<font color=\"red\">[nick]</font>: " + text);
}

void ClientWindow::on_tb_chat_anchorClicked(const QUrl &arg1)
{
    if(arg1.toString()=="#connectCoordinator") {
        connectToCoordinator();
    }
}
