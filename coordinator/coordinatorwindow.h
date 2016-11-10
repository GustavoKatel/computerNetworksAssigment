#ifndef COORDINATORWINDOW_H
#define COORDINATORWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

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
    void on_pushButton_clicked();

    void readPendingDatagrams();

private:
    Ui::CoordinatorWindow *ui;

    QUdpSocket *_socket;

    void processDatagram(QHostAddress &senderAddr, int senderPort, QByteArray *data);

    void log(QString &msg);
    void log(const QString &msg);
};

#endif // COORDINATORWINDOW_H
