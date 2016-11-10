#include "coordinator/coordinatorwindow.h"
#include "ui_coordinatorwindow.h"

#include <QMessageBox>

CoordinatorWindow::CoordinatorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CoordinatorWindow),
    _socket(nullptr)
{
    ui->setupUi(this);

    ui->pushButton_2->hide();

    QHostAddress anyAddr(QHostAddress::Any);
    ui->le_addr->setText(anyAddr.toString());
}

CoordinatorWindow::~CoordinatorWindow()
{
    delete ui;
}

void CoordinatorWindow::on_pushButton_clicked()
{
    QHostAddress addr(ui->le_addr->text());

    if(addr.isNull() || ui->le_addr->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please inform a valid address", QMessageBox::Ok);
        return;
    }

    QString portStr = ui->le_port->text();
    bool ok = false;
    int port = portStr.toInt(&ok);
    if(portStr.isEmpty() || !ok) {
        QMessageBox::warning(this, "Error", "Please inform a valid port", QMessageBox::Ok);
        return;
    }

    ui->pushButton_2->show();
    ui->pushButton->hide();

    _socket = new QUdpSocket(this);
    _socket->bind(addr, port);

    connect(_socket, SIGNAL(readyRead()),
                this, SLOT(readPendingDatagrams()));
}

void CoordinatorWindow::readPendingDatagrams()
{
    while (_socket->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        datagram->resize(_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        _socket->readDatagram(datagram->data(), datagram->size(),
                                &sender, &senderPort);

        processDatagram(sender, senderPort, datagram);
    }
}

void  CoordinatorWindow::processDatagram(QHostAddress &senderAddr, int senderPort, QByteArray *data)
{
    QString line(*data);
    line = "from: " + senderAddr.toString()+":" + senderPort + " " + " data: " + line;
    log( line );
}

void CoordinatorWindow::log(QString &msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}

void CoordinatorWindow::log(const QString &msg)
{
    log( msg );
}
