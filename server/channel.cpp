#include "channel.h"
#include <QVBoxLayout>

Channel::Channel(const QString &name, QTabWidget *tabParent)
{
    this->name = name;
    this->users = QList<QTcpSocket*>();

    this->tab = initializeTab(tabParent);
}

QWidget* Channel::initializeTab(QTabWidget *tabParent)
{
    // Initialize tab
    QWidget *tab = new QWidget();
    tab->setObjectName(name);

    // Create text area
    textEditLog = new QPlainTextEdit();
    textEditLog->setObjectName(QStringLiteral("textEditLog"));
    textEditLog->setEnabled(false);
    textEditLog->appendPlainText("lol");

    // Create layout that will contain text area
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textEditLog);

    // Add layout to tab
    tab->setLayout(layout);

    // Add tab to tabParent
    tabParent->addTab(tab, QString());
    tabParent->setTabText(tabParent->indexOf(tab), name);

    return tab;
}

void Channel::addUser(QTcpSocket *user)
{
    users.append(user);
    log("User joined the channel: " + user->peerAddress().toString() + " " + QString::number(user->peerPort()));
}

void Channel::log(QString message)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    textEditLog->appendPlainText(
                currentTime.toString() + ": " + message.trimmed());
}
