#include "utils.h"

#include <QNetworkInterface>

QStringList Utils::testRegexAndCapture(QString regex, QString text)
{
    QRegExp re(regex);

    QStringList captures;

    // not a match
    if(!re.exactMatch(text)) {
        return captures;
    }

    return re.capturedTexts();

}

bool Utils::testRegex(QString regex, QString text)
{
    QRegExp re(regex);

    return re.exactMatch(text);

}


QHostAddress Utils::getFirstNonLocalhost(bool onlyIPv4, QHostAddress::SpecialAddress fallback)
{
    QHostAddress addr(fallback);

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    if(!onlyIPv4) {

        // searches the first non-localhost IPv6 address
        for(auto ip : ipAddressesList) {

            // skip not-ipv6
            if( ip.protocol() != QAbstractSocket::IPv6Protocol )
                continue;

            if( ip != QHostAddress::LocalHostIPv6 )
                return ip;

        }

    }

    // searches the first non-localhost IPv4 address
    for(auto ip : ipAddressesList) {

        // skip not-ipv4
        if( ip.protocol() != QAbstractSocket::IPv4Protocol )
            continue;

        if( ip != QHostAddress::LocalHost )
            return ip;

    }

    // no ip found, return fallback
    return addr;

}
