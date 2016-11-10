#include "coordinator/coordinatorwindow.h"
#include "ui_coordinatorwindow.h"

CoordinatorWindow::CoordinatorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CoordinatorWindow),
    _socket(this)
{
    ui->setupUi(this);
}

CoordinatorWindow::~CoordinatorWindow()
{
    delete ui;
}

void CoordinatorWindow::on_pushButton_clicked()
{
    // TODO: start udp socket
}
