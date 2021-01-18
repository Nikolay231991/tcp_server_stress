#ifndef CONNECTIONSSTRESS_H
#define CONNECTIONSSTRESS_H

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
#include <mutex>

#include <QObject>



/* Wrapper for connect socket function */
extern int Connect(int, struct sockaddr*, socklen_t);

class ConnectionsStress: public QObject
{
    Q_OBJECT

public:
    ConnectionsStress();

    void stressThread();

    ~ConnectionsStress();

public slots:
    void makeStress(bool);

    void stopStress(bool);

signals:
    void connectionsCounter(int);
    void result(bool);

private:
    static constexpr int MAX_CONNS = 5;
    static std::mutex activeConnsMutex_;
    std::atomic<bool> stressRunning_;
    std::shared_ptr<std::thread> stressThread_;
    int activeConns_;

    void clientThread_(int);
};


#endif // CONNECTIONSSTRESS_H
