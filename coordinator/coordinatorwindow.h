#ifndef COORDINATORWINDOW_H
#define COORDINATORWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QHash>

#include "coordinator/server.h"

namespace Ui {
class CoordinatorWindow;
}

class CoordinatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CoordinatorWindow(QWidget *parent = 0);
    ~CoordinatorWindow();

private slots:
    void read_pending_datagrams();

    void socket_changed_state(QAbstractSocket::SocketState state);

    void on_bt_start_clicked();

    void on_bt_stop_clicked();

private:
    Ui::CoordinatorWindow *ui;

    QUdpSocket *_socket;

    QHash<QString, Server *> _serverList;

    void processDatagram(QHostAddress &senderAddr, int senderPort, QByteArray *data);

    void log(QString msg);

    void udpSend(QHostAddress &addr, int port, QString &data);

    // protocol functions
    void sendServer(QHostAddress &addr, int port);

};

#endif // COORDINATORWINDOW_H
