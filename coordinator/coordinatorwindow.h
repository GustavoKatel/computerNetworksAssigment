#ifndef COORDINATORWINDOW_H
#define COORDINATORWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QHash>
#include <QTreeWidgetItem>

#include "dataclass/d_server.h"
#include "dataclass/d_channel.h"
#include "protocol/protocol.h"

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

    QHash<QString, ServerData *> _serverList;
    QHash<QString, QTreeWidgetItem *> _serverTreeMap;
    QHash<QString, QList<ChannelData *>> _channelMap;

    ProtocolParser _parser;

    void processDatagram(QHostAddress &senderAddr, int senderPort, QByteArray *data);

    void log(const QString &msg);

    void udpSend(QHostAddress &addr, int port, const QString &data);

    void updateServerListView();

    // protocol functions
    void sendServer(QHostAddress &addr, int port);
    void registerChannels(QHostAddress &senderAddr, int senderPort);
    void sendChannels(QHostAddress &addr, int port);
    void join(QHostAddress &addr, int port);

};

#endif // COORDINATORWINDOW_H
