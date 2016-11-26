#include "channel.h"
#include <QVBoxLayout>
#include <QScrollBar>

ChannelData::ChannelData(const QString &name, QTabWidget *tabParent) : QObject()
{
    this->name = name;
    this->users = QList<QTcpSocket*>();

    this->tab = initializeTab(tabParent);
}

QWidget* ChannelData::initializeTab(QTabWidget *tabParent)
{
    // Initialize tab
    QWidget *tab = new QWidget(tabParent);
    tab->setObjectName(name);

    // Create text area
    textEditLog = new QPlainTextEdit();
    textEditLog->setObjectName(QStringLiteral("textEditLog"));
    textEditLog->setEnabled(false);

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

void ChannelData::addUser(QTcpSocket *user)
{
    // Notify all users that someone joined the channel
    sendMessage("User joined the channel\n");

    // Add user list of users
    users.append(user);

    // Log on tab
    log("User joined the channel: " + user->peerAddress().toString() + " " + QString::number(user->peerPort()));

    // Remove signal from slot. Channel should take care of messages now
    disconnect(user, &QIODevice::readyRead, 0, 0);

    // Add handler to new incoming messages
    connect(user, &QIODevice::readyRead,
            this, [this, user]{ on_readyRead(user); });

    connect(user, &QTcpSocket::disconnected,
            this, [this, user]{ on_disconnected(user); });
}

void ChannelData::sendMessage(QString message)
{
    log("Message received: " + message);

    QListIterator<QTcpSocket*> iterator(users);
    while (iterator.hasNext()) {
        QTcpSocket *user = iterator.next();

        user->write(message.toUtf8());
        user->waitForBytesWritten();
    }

}

void ChannelData::log(QString message)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    textEditLog->appendPlainText(
                currentTime.toString() + ": " + message.trimmed());

    // Auto scroll to bottom
    QScrollBar *sb = textEditLog->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void ChannelData::on_readyRead(QTcpSocket *tcpSocket)
{
    if(tcpSocket->canReadLine())
    {
        QByteArray biteArray = tcpSocket->readLine();
        sendMessage(biteArray);
    }
}

void ChannelData::on_disconnected(QTcpSocket *tcpSocket)
{
    users.removeOne(tcpSocket);

    sendMessage("Some user disconnected\n");
    log("Some user disconnected");
}
