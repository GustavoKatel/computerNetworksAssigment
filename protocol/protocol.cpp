#include "protocol.h"

#include "utils.h"

#define REGEX_QUERY_SERVER "^GET SERVER$"
#define REGEX_SERVER_ADD "^SERVER ADD ([\\.0-9:\\[\\]]+) (\\d+)$"
#define REGEX_JOIN "^JOIN (\\w+)$"
#define REGEX_SERVER_INFO "^SERVER INFO (.+) (\\d+)$"
#define REGEX_GET_CHANNELS "^GET CHANNELS$"
#define REGEX_CHANNEL_INFO "^CHANNEL INFO (\\w+)$"
#define REGEX_OK "^OK$"

#define FMT_SERVER_ADD "SERVER ADD $1 $2"
#define FMT_JOIN "JOIN $1"
#define FMT_SERVER_INFO "SERVER INFO $1 $2"
#define FMT_CHANNEL_INFO "CHANNEL INFO $1"

ProtocolParser::ProtocolParser(QObject *parent) :
    QObject(parent)
{

}

bool ProtocolParser::parse(const QString &data)
{
    this->_data = data;

    _channels.clear();

    QStringList lines = data.split('\n');

    bool result = true;
    for(auto line : lines) {
        result &= parseLine(line);
    }

    return result;
}

QString ProtocolParser::make(const ProtocolMethod &method, const QStringList &args)
{
    switch (method) {
    case QUERY_SERVER:
        return "GET SERVER";
        break;
    case JOIN:
        // return QString(FMT_JOIN).arg()
        break;
    case JOIN:
        break;
    case SERVER_ADD:
        break;
    case SERVER_INFO:
        break;
    case GET_CHANNELS:
        break;
    case CHANNEL_INFO:
        break;
    case OK:
        break;
    case NO_METHOD:
        return QString("");
    default:
        break;
    }
}

bool ProtocolParser::parseLine(const QString &data)
{

    if(Utils::testRegex(REGEX_GET_CHANNELS, data)) {
        this->_currentMethod = GET_CHANNELS;
        _args = Utils::testRegexAndCapture(REGEX_GET_CHANNELS, data);
        return true;
    }

    if(Utils::testRegex(REGEX_JOIN, data)) {
        this->_currentMethod = JOIN;
        _args = Utils::testRegexAndCapture(REGEX_JOIN, data);
        return true;
    }

    if(Utils::testRegex(REGEX_OK, data)) {
        this->_currentMethod = OK;
        _args = Utils::testRegexAndCapture(REGEX_OK, data);
        return true;
    }

    if(Utils::testRegex(REGEX_QUERY_SERVER, data)) {
        this->_currentMethod = QUERY_SERVER;
        _args = Utils::testRegexAndCapture(REGEX_QUERY_SERVER, data);
        return true;
    }

    if(Utils::testRegex(REGEX_SERVER_ADD, data)) {
        this->_currentMethod = SERVER_ADD;
        _args = Utils::testRegexAndCapture(REGEX_SERVER_ADD, data);
        return true;
    }

    if(Utils::testRegex(REGEX_SERVER_INFO, data)) {
        this->_currentMethod = SERVER_INFO;
        _args = Utils::testRegexAndCapture(REGEX_SERVER_INFO, data);
        return true;
    }

    if(Utils::testRegex(REGEX_CHANNEL_INFO, data)) {
        this->_currentMethod = CHANNEL_INFO;
        _args = Utils::testRegexAndCapture(REGEX_CHANNEL_INFO, data);
        for(auto arg : _args) {
            _channels.push_back(arg);
        }
        return true;
    }

    this->_currentMethod = NO_METHOD;
    return false;
}

const ProtocolMethod &ProtocolParser::getMethod() const
{
    return _currentMethod;
}

const QStringList &ProtocolParser::getArgs() const
{
    return _args;
}
