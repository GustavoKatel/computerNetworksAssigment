#ifndef CHANNEL_H
#define CHANNEL_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include <QtNetwork>

class Channel
{
public:
    Channel(const QString &name, QTabWidget *tabParent);

    void addUser(QTcpSocket *user);

private:
    QString name;
    QWidget *tab;
    QList<QTcpSocket*> users;

    QWidget* initializeTab(QTabWidget *tabParent);
};

#endif // CHANNEL_H
