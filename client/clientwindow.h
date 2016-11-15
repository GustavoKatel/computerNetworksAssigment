#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QUdpSocket>

#include "client/connecttocoordinatordialog.h"
#include "coordinator/coordinatorclient.h"

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = 0);
    ~ClientWindow();

private slots:
    void coordinator_changed_state(QAbstractSocket::SocketState state);

    void server_info(QList<ServerData *> servers);

    void on_le_text_returnPressed();

    void on_toolButton_clicked();

    void on_tb_chat_anchorClicked(const QUrl &arg1);

private:
    Ui::ClientWindow *ui;

    QTcpSocket *_socketServer;

    // Coordinator
    CoordinatorClient *_coordinatorClient;
    QHostAddress _coordinatorAddr;
    int _coordinatorPort;

    ConnectToCoordinatorDialog *_connectToCoordinatorDialog;

    void log(const QString &msg);

    void initChat();

    void initCoordinator();

    void connectToCoordinator();

    void processCommand(const QString &line);

    void sendText(const QString &text);
};

#endif // CLIENTWINDOW_H
