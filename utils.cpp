#include "utils.h"

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
