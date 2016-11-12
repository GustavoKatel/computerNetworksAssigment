#include "channel.h"

Channel::Channel(const QString &name, QTabWidget *tabParent)
{
    this->name = name;
    this->tab = initializeTab(tabParent);
    this->users = QList<QTcpSocket*>();
}

QWidget* Channel::initializeTab(QTabWidget *tabParent)
{
    QWidget *tab = new QWidget();
    tab->setObjectName(name);
    tabParent->addTab(tab, QString());
    tabParent->setTabText(tabParent->indexOf(tab), name);

    return tab;
}

void Channel::addUser(QTcpSocket *user)
{
    users.append(user);
    qDebug() << "adding user";
}

