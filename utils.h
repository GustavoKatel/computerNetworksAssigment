#ifndef UTILS_H
#define UTILS_H

#include <QStringList>
#include <QRegExp>
#include <QHostAddress>

namespace Utils {

/**
     * @brief testRegexAndCapture Test regex against text and return the captures
     * @param regex QString regex to test
     * @param text QString text to be tested
     * @return QStringList the list of captures defined by the regex
     */
    QStringList testRegexAndCapture(QString regex, QString text);

    /**
     * @brief testRegex Test regex against text
     * @param regex QString regex to test
     * @param text QString text to be tested
     * @return bool True if the regex matches with text
     */
    bool testRegex(QString regex, QString text);


    /**
     * @brief getFirstNonLocalhost Returns the first non-localhost IPv6 address. Fallback to ipv4.
     * returns an empty address if none is available. Respects the onlyIPv4 param.
     * @param onlyIPv4 Ignore IPv6
     * @param fallback The default address if no criteria matches
     * @return QHostAddress The chosen address
     */
    QHostAddress getFirstNonLocalhost(bool onlyIPv4=false, QHostAddress::SpecialAddress fallback=QHostAddress::Any);

}

#endif // UTILS_H
