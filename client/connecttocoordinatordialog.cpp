#include "connecttocoordinatordialog.h"
#include "ui_connecttocoordinatordialog.h"

ConnectToCoordinatorDialog::ConnectToCoordinatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToCoordinatorDialog)
{
    ui->setupUi(this);
}

ConnectToCoordinatorDialog::~ConnectToCoordinatorDialog()
{
    delete ui;
}
