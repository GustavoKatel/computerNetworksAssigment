#include "connecttocoordinatordialog.h"
#include "ui_connecttocoordinatordialog.h"

#include "utils.h"

ConnectToCoordinatorDialog::ConnectToCoordinatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToCoordinatorDialog)
{
    ui->setupUi(this);

    ui->le_address->setText(Utils::getFirstNonLocalhost(true).toString());
}

ConnectToCoordinatorDialog::~ConnectToCoordinatorDialog()
{
    delete ui;
}

const QString ConnectToCoordinatorDialog::getAddress() const
{
    return ui->le_address->text();
}

int ConnectToCoordinatorDialog::getPort() const {
    return ui->le_port->text().toInt();
}
