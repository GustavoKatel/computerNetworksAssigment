#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

#include "client/connecttocoordinatordialog.h"

#include "server/channel.h"
#include "server/channelslist.h"

#include "protocol/protocol.h"

#include "coordinator/coordinatorclient.h"

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = 0);
    ~ServerWindow();

private slots:

private:
    Ui::ServerWindow *ui;
    ChannelsList *channelsList;

    QTcpServer *tcpServer;
    QDataStream in;

    ProtocolParser _parser;

    // Coordinator
    CoordinatorClient *_coordinatorClient;
    QHostAddress _coordinatorAddr;
    int _coordinatorPort;

    ConnectToCoordinatorDialog *_connectToCoordinatorDialog;

    void initCoordinator();
    void connectToCoordinator();

    // Called after we receive OK from coordinator
    // Draw the GUI and start everything
    void startServer();

    void startTCPServer();
    void on_readyRead(QTcpSocket *tcpSocket);
    void handleConnection();

    void createChannel(QString channelName);
    void addUserToChannel(QTcpSocket *user, QString channelName);

    void destroyChannel();

    void log(QString message);
};

#endif // SERVERWINDOW_H
