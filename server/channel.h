#ifndef CHANNEL_H
#define CHANNEL_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtNetwork>

class Channel
{
public:
    Channel(const QString &name, QTabWidget *tabParent);

    void addUser(QTcpSocket *user);

private:
    QString name;
    QWidget *tab;
    QPlainTextEdit *textEditLog;
    QList<QTcpSocket*> users;

    QWidget* initializeTab(QTabWidget *tabParent);
    void log(QString message);
};

#endif // CHANNEL_H
