#ifndef SERVER_H
#define SERVER_H

#include <QHostAddress>

class Server {

public:
    Server(QHostAddress &addr, int port, int totalClients=0);

    /**
     * @brief getAddress Getter: Server address
     * @return QHostAddress wrapping the host address of the server
     */
    const QHostAddress &getAddress() const;

    /**
     * @brief getPort Getter: Server port
     * @return int the server port
     */
    int getPort() const;

    /**
     * @brief getTotalClients The total number of clientes in that server
     * @return int count of clientes
     */
    int getTotalClients() const;

    /**
     * @brief incClients Increment the total number of clientes in that server
     * @return int the new count of clientes
     */
    int incClients();

    /**
     * @brief decClients Decrement the totala number fo clientes in that server
     * @return int the new count of clientes
     */
    int decClients();

private:
    QHostAddress _addr;
    int _port;
    int _totalClients;

};

#endif // SERVER_H
