#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <exception>
#include <thread>
#include <memory>
#include <string>
#include <atomic>
#include <functional>
#include <chrono>

#include <QObject>


/* Wrapper for connect socket function */
int Connect(int, struct sockaddr*, socklen_t);

class TcpClient: public QObject
{
    Q_OBJECT

public:
    TcpClient(std::string ip_address = "192.168.42.1", int port = 3425);

    void tcp_connect();

    void disconnect();

    inline bool isConnected()
    {
        return isConnActive_.load();
    }

    inline int getCounter()
    {
        return counter_.load();
    }

    ~TcpClient();

signals:
    void changedCounter(int);
    void disconnection();

private:
    static constexpr size_t COUNTER_SIZE = 10;
    std::string ip_address_;
    int port_;
    int sock_;

    std::atomic<bool> isConnActive_;
    std::atomic<int> counter_;

    std::shared_ptr<std::thread> client_thread_;

    void clientThread_(int conn_socket);
};


#endif // TCPCLIENT_H
