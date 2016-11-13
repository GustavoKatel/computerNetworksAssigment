#ifndef CONNECTTOCOORDINATORDIALOG_H
#define CONNECTTOCOORDINATORDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectToCoordinatorDialog;
}

class ConnectToCoordinatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectToCoordinatorDialog(QWidget *parent = 0);
    ~ConnectToCoordinatorDialog();

    const QString getAddress() const;
    int getPort() const;

private:
    Ui::ConnectToCoordinatorDialog *ui;
};

#endif // CONNECTTOCOORDINATORDIALOG_H
