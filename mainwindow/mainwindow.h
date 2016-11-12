#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "client/clientwindow.h"
#include "coordinator/coordinatorwindow.h"
#include "server/serverwindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_buttonServer_clicked();
    void on_buttonCoordinator_clicked();

private:
    Ui::MainWindow *ui;

    CoordinatorWindow * coordinatorWindow;
    ServerWindow * serverWindow;
};

#endif // MAINWINDOW_H
