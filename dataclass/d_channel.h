#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include <QHostAddress>

class ChannelData {

public:
    ChannelData(const QString &name, const QHostAddress &host, int port);

    const QString &getName() const;
    void setName(const QString &name);

    const QHostAddress &getAddress() const;
    void setAddress(const QHostAddress &host);

    int getPort() const;
    void setPort(int port);

private:
    QString _name;
    QHostAddress _addr;
    int _port;

};

#endif // CHANNEL_H
