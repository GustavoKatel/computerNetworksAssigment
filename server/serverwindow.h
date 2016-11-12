#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

#include "server/channel.h"
#include "server/channelslist.h"

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

    void startTCPServer();
    void on_readyRead(QTcpSocket *tcpSocket);
    void handleConnection();

    void createChannel(QString channelName);
    void addUserToChannel(QTcpSocket *user, QString channelName);

    void destroyChannel();

    void log(QString message);
};

#endif // SERVERWINDOW_H
