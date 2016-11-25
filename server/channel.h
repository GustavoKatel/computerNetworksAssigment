#ifndef CHANNEL_H
#define CHANNEL_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtNetwork>

class ChannelData : public QObject
{
    Q_OBJECT

public:
    ChannelData(const QString &name, QTabWidget *tabParent);

    void addUser(QTcpSocket *user);
    void sendMessage(QString message);

private:
    QString name;
    QWidget *tab;
    QPlainTextEdit *textEditLog;
    QList<QTcpSocket*> users;

    QWidget* initializeTab(QTabWidget *tabParent);

    void on_readyRead(QTcpSocket *tcpSocket);
    void on_disconnected(QTcpSocket *tcpSocket);

    void log(QString message);
};

#endif // CHANNEL_H
