#ifndef UTILS_H
#define UTILS_H

#include <QStringList>
#include <QRegExp>

namespace Utils {

    QStringList testRegexAndCapture(QString regex, QString text);
    bool testRegex(QString regex, QString text);

}

#endif // UTILS_H
