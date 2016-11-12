#include "channel.h"

Channel::Channel(const QString &name, QTabWidget *tabParent)
{
    this->name = name;
    this->tab = initializeTab(tabParent);
}

QWidget* Channel::initializeTab(QTabWidget *tabParent)
{
    QWidget *tab = new QWidget();
    tab->setObjectName(name);
    tabParent->addTab(tab, QString());
    tabParent->setTabText(tabParent->indexOf(tab), name);

    return tab;
}

