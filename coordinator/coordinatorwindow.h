#ifndef COORDINATORWINDOW_H
#define COORDINATORWINDOW_H

#include <QMainWindow>
#include <QThread>
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

private:
    Ui::CoordinatorWindow *ui;

    QThread _socketThread;
    QUdpSocket _socket;
};

#endif // COORDINATORWINDOW_H
