#include "mainwindow/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonServer_clicked()
{
    this->hide();
    serverWindow = new ServerWindow(this);
    serverWindow->show();
}

void MainWindow::on_buttonCoordinator_clicked()
{
    this->hide();
    coordinatorWindow = new CoordinatorWindow(this);
    coordinatorWindow->show();
}

void MainWindow::on_buttonClient_clicked()
{
    this->hide();
    clientWindow = new ClientWindow(this);
    clientWindow->show();
}
