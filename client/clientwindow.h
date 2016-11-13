#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QUdpSocket>

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
    void coordinator_read_pending_datagrams();

    void coordinator_changed_state(QAbstractSocket::SocketState state);

    void on_le_text_returnPressed();

    void on_toolButton_clicked();

    void on_tb_chat_anchorClicked(const QUrl &arg1);

private:
    Ui::ClientWindow *ui;

    QTcpSocket *_socketServer;
    QUdpSocket *_socketCoordinator;

    void log(const QString &msg);

    void initChat();

    void initCoordinator();

    void processCommand(const QString &line);

    void sendText(const QString &text);
};

#endif // CLIENTWINDOW_H
