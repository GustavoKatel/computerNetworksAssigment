#include "protocol.h"

#include "utils.h"

#define REGEX_QUERY_SERVER "^GET SERVER$"
#define REGEX_SERVER_ADD "^SERVER ADD ([a-zA-Z0-9\\:\\.]+) (\\d+)$"
#define REGEX_JOIN "^JOIN (\\w+)$"
#define REGEX_SERVER_INFO "^SERVER INFO (.+) (\\d+)$"
#define REGEX_GET_CHANNELS "^GET CHANNELS$"
#define REGEX_CHANNEL_INFO "^CHANNEL INFO (\\w+) ([a-zA-Z0-9\\:\\.]+) (\\d+)$"
#define REGEX_OK "^OK$"

#define FMT_SERVER_ADD "SERVER ADD %1 %2"
#define FMT_JOIN "JOIN %1"
#define FMT_SERVER_INFO "SERVER INFO %1 %2"
#define FMT_CHANNEL_INFO "CHANNEL INFO %1 %2 %3"

ProtocolParser::ProtocolParser(QObject *parent) :
    QObject(parent),
    _methods(),
    _args(),
    _servers(),
    _channels()
{

}

bool ProtocolParser::parse(const QString &data)
{
    this->_data = data;

    _args.clear();
    _methods.clear();

    _servers.clear();
    _channels.clear();

    QStringList lines = data.split('\n');

    bool result = true;
    for(auto line : lines) {
        if(line.isEmpty()) continue;
        result &= parseLine(line);
    }

    return result;
}

QString ProtocolParser::make(const ProtocolMethod &method, const QStringList &args)
{
    QString res;
    switch (method) {
    case GET_SERVER:
        return "GET SERVER";

    case JOIN:
        res = FMT_JOIN;
        for(auto arg : args) {
            res = res.arg(arg);
        }
        return res;

    case SERVER_ADD:
        res = FMT_SERVER_ADD;
        for(auto arg : args) {
            res = res.arg(arg);
        }
        return res;

    case SERVER_INFO:
        res = FMT_SERVER_INFO;
        for(auto arg : args) {
            res = res.arg(arg);
        }
        return res;

    case GET_CHANNELS:
        return "GET CHANNELS";

    case CHANNEL_INFO:
        res = FMT_CHANNEL_INFO;
        for(auto arg : args) {
            res = res.arg(arg);
        }
        return res;

    case OK:
        return "OK";

    case NO_METHOD:
    default:
        return "";
    }
}

QString ProtocolParser::makeBatch(const QList<ProtocolMethod> &methods, const QList<QStringList> &argList)
{
    if(methods.length() != argList.length()) {
        // arglist's size does not match methods' size
        return "";
    }

    ProtocolMethod method;
    QStringList args;

    QString res = "";

    for(int i=0;i<methods.length();i++) {

        method = methods[i];
        args = argList[i];

        res += this->make(method, args);

    }

    return res;
}

QString ProtocolParser::make_CHANNEL_INFO(ChannelData * channel)
{
    return make(ProtocolMethod::CHANNEL_INFO,
                QStringList() << channel->getName()
                                << channel->getAddress().toString()
                                << QString::number(channel->getPort())
                );
}

QString ProtocolParser::make_CHANNEL_INFO(const QList<ChannelData *> &channels)
{
    QString res = "";

    for(auto channel : channels) {

        res += make_CHANNEL_INFO(channel) + "\n";

    }

    return res;
}

bool ProtocolParser::parseLine(const QString &data)
{

    QStringList args;
    ProtocolMethod method;
    bool res = false;

    if(Utils::testRegex(REGEX_GET_CHANNELS, data)) {
        method = GET_CHANNELS;
        args = Utils::testRegexAndCapture(REGEX_GET_CHANNELS, data);
        res = true;

    } else if(Utils::testRegex(REGEX_JOIN, data)) {
        method = JOIN;
        args = Utils::testRegexAndCapture(REGEX_JOIN, data);
        res = true;

    } else if(Utils::testRegex(REGEX_OK, data)) {
        method = OK;
        args = Utils::testRegexAndCapture(REGEX_OK, data);
        res = true;

    } else if(Utils::testRegex(REGEX_QUERY_SERVER, data)) {
        method = GET_SERVER;
        args = Utils::testRegexAndCapture(REGEX_QUERY_SERVER, data);
        res = true;

    } else if(Utils::testRegex(REGEX_SERVER_ADD, data)) {
        method = SERVER_ADD;
        args = Utils::testRegexAndCapture(REGEX_SERVER_ADD, data);
        res = true;

    } if(Utils::testRegex(REGEX_SERVER_INFO, data)) {
        method = SERVER_INFO;
        args = Utils::testRegexAndCapture(REGEX_SERVER_INFO, data);
        res = true;

    } if(Utils::testRegex(REGEX_CHANNEL_INFO, data)) {
        method = CHANNEL_INFO;
        args = Utils::testRegexAndCapture(REGEX_CHANNEL_INFO, data);
        res = true;
    }

    if(!res) {
        method = NO_METHOD;
    }

    _args.push_back(args);
    _methods.push_back(method);

    return res;
}

const QList<ProtocolMethod> &ProtocolParser::getMethods() const
{
    return _methods;
}

const QList<QStringList> &ProtocolParser::getArgs() const
{
    return _args;
}

const QList<ServerData *> &ProtocolParser::getServers()
{
    QStringList argLine;
    ProtocolMethod method;

    if(_servers.length()>0) return _servers;

    for(int i=0;i<_methods.length();i++) {

        argLine = _args[i];
        method = _methods[i];

        if( ( method!=ProtocolMethod::SERVER_INFO  &&
                method!=ProtocolMethod::SERVER_ADD )  ||
                argLine.length() != 3
                ) {
            continue;
        }

        ServerData *server = new ServerData(QHostAddress(argLine[1]), argLine[2].toInt());
        _servers.push_back(server);

    }

    return _servers;

}

const QList<ChannelData *> &ProtocolParser::getChannels()
{
    QStringList argLine;
    ProtocolMethod method;

    if(_channels.length()>0) return _channels;

    for(int i=0;i<_methods.length();i++) {

        argLine = _args[i];
        method = _methods[i];

        if(method!=ProtocolMethod::CHANNEL_INFO || argLine.length() != 4) {
            continue;
        }

        ChannelData *channel = new ChannelData(argLine[1], QHostAddress(argLine[2]), argLine[3].toInt());
        _channels.push_back(channel);

    }

    return _channels;

}

QString ProtocolParser::make_GET_SERVER()
{
    return make(ProtocolMethod::GET_SERVER, QStringList());
}

QString ProtocolParser::make_JOIN(const QString &channelName)
{
    return make(ProtocolMethod::JOIN, QStringList(channelName));
}

QString ProtocolParser::make_SERVER_ADD(const QHostAddress &addr, int port)
{
    return make(ProtocolMethod::SERVER_ADD, QStringList() << addr.toString() << QString::number(port));
}

QString ProtocolParser::make_GET_CHANNELS()
{
    return make(ProtocolMethod::GET_CHANNELS, QStringList());
}

QString ProtocolParser::make_OK()
{
    return "OK";
}

QString ProtocolParser::make_SERVER_INFO(ServerData *server)
{
    return make(ProtocolMethod::SERVER_INFO, QStringList() << server->getAddress().toString() << QString::number(server->getPort()));
}
