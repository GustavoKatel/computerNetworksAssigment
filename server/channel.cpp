#include "channel.h"
#include <QVBoxLayout>

Channel::Channel(const QString &name, QTabWidget *tabParent) :
    QObject()
{
    this->name = name;
    this->users = QList<QTcpSocket*>();

    this->tab = initializeTab(tabParent);
}

QWidget* Channel::initializeTab(QTabWidget *tabParent)
{
    // Initialize tab
    QWidget *tab = new QWidget(tabParent);
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
    // Notify all users that someone joined the channel
    sendMessage("User joined the channel");

    // Add user list of users
    users.append(user);

    // Log on tab
    log("User joined the channel: " + user->peerAddress().toString() + " " + QString::number(user->peerPort()));

    // Add handler to new incoming messages
    connect(user, &QIODevice::readyRead,
            this, [this, user]{ on_readyRead(user); });
}

void Channel::sendMessage(QString message)
{
    QListIterator<QTcpSocket*> iterator(users);
    while (iterator.hasNext()) {
        QTcpSocket *user = iterator.next();

        user->write(message.toUtf8());
        user->waitForBytesWritten();
    }

}

void Channel::log(QString message)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    textEditLog->appendPlainText(
                currentTime.toString() + ": " + message.trimmed());
}

void Channel::on_readyRead(QTcpSocket *tcpSocket)
{
    if(tcpSocket->canReadLine())
    {
        QByteArray biteArray = tcpSocket->readLine();
        sendMessage(biteArray);
    }
}
