#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = 0);
    void startTCPServer();
    void createChannel();
    void destroyChannel();
    void echo(QTcpSocket *tcpSocket);
    void handleConnection();

    ~ServerWindow();

private:
    Ui::ServerWindow *ui;
    QTcpServer *tcpServer;
    QDataStream in;

    void log(QString message);
};

#endif // SERVERWINDOW_H
