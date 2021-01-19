#include "tcpclient.h"



TcpClient::TcpClient(std::string ip_address, int port):
    ip_address_(ip_address),
    port_(port),
    sock_(-1),
    isConnActive_(false),
    counter_(0),
    client_thread_(nullptr)
{


}

void TcpClient::tcp_connect()
{
    if (client_thread_ == nullptr)
    {
        sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_ < 0)
        {
            throw std::runtime_error("socket creation failed\n");
        }

        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof (serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port_);
        serv_addr.sin_addr.s_addr = inet_addr(ip_address_.c_str());

        int status = Connect(sock_, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof (serv_addr));
        if (status < 0)
        {
            throw std::runtime_error("tcp connection refused\n");
        }

        isConnActive_.store(true);
        client_thread_ = std::make_shared<std::thread>(&TcpClient::clientThread_, this, sock_);

        if (client_thread_ == nullptr)
        {
            throw std::runtime_error("tcp client thread creation error\n");
        }
    }
    else
    {
        throw std::runtime_error("tcp client is already connected\n");
    }
}

void TcpClient::disconnect()
{
    isConnActive_.store(false);

    if (client_thread_ != nullptr)
    {
        if (client_thread_->joinable())
        {
            client_thread_->join();
        }
        client_thread_.reset();
    }

}




TcpClient::~TcpClient()
{
    disconnect();
}

void TcpClient::clientThread_(int conn_socket)
{
    std::string str_counter = "test:counter:" + std::to_string(counter_);
    try
    {
        int status = send(conn_socket, str_counter.c_str(), str_counter.size(), 0);
        if (status < 0)
        {
            emit disconnection();
            throw std::runtime_error("disconnected");
        }

        char buff[COUNTER_SIZE];
        while (isConnActive_.load())
        {
            int bytes = recv(conn_socket, buff, COUNTER_SIZE, 0);

            if (bytes <= 0)
            {
                isConnActive_.store(false);
                emit disconnection();
            }
            else
            {
                int value = atoi(buff);
                std::string tmp = "test:counter:" + std::string(buff);
                counter_.store(value);
                emit changedCounter(counter_);
                if (send(conn_socket, tmp.c_str(), tmp.size(), 0) < 0)
                {
                    emit disconnection();
                    throw std::runtime_error("disconnected");
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
         close(conn_socket);
    }
    catch (const std::exception& e)
    {
        close(conn_socket);
    }
}

int Connect(int sock, struct sockaddr* addr, socklen_t len)
{
    return connect(sock, addr, len);
}
