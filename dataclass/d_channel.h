#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>

class ChannelData {

public:
    ChannelData(const QString &name);

    const QString &getName() const;

    void setName(const QString &name);

private:
    QString _name;

};

#endif // CHANNEL_H
