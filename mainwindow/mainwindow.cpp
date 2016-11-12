#include "mainwindow/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    coordinatorWindow = new CoordinatorWindow(this);
    serverWindow = new ServerWindow(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonServer_clicked()
{
    this->hide();
    serverWindow->show();
}

void MainWindow::on_buttonCoordinator_clicked()
{
    this->hide();
    coordinatorWindow->show();
}
