#include "connectionsstress.h"

std::mutex ConnectionsStress::activeConnsMutex_;



ConnectionsStress::ConnectionsStress():
    stressRunning_(false),
    stressThread_(nullptr),
    activeConns_(0)
{

}

void ConnectionsStress::makeStress(bool)
{
    if (stressThread_ == nullptr)
    {
        stressRunning_ .store(true);
        stressThread_ = std::make_shared<std::thread>(&ConnectionsStress::stressThread, this);
    }
}

void ConnectionsStress::stopStress(bool)
{
    if (stressThread_ != nullptr)
    {
        stressRunning_ .store(false);

        if (stressThread_->joinable())
        {
            stressThread_->join();
            activeConns_ = 0;
            emit connectionsCounter(activeConns_);
        }

        stressThread_.reset();
    }
}

void ConnectionsStress::stressThread()
{
   std::vector<int> newSockets;
   newSockets.reserve(10);
   std::vector<int> connectedSockets;
   connectedSockets.reserve(5);

   struct sockaddr_in serv_addr;
   memset(&serv_addr, 0, sizeof (serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(3425);
   serv_addr.sin_addr.s_addr = inet_addr("192.168.42.1");

   while (stressRunning_.load())
   {
       int new_socket = socket(AF_INET, SOCK_STREAM, 0);
       if (new_socket > 0)
       {
           int status = Connect(new_socket, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof (serv_addr));

           if (status >= 0)
           {
               std::thread t(&ConnectionsStress::clientThread_, this, new_socket);
               t.detach();
           }

           std::this_thread::sleep_for(std::chrono::milliseconds(200));

           emit connectionsCounter(activeConns_);
           std::lock_guard<std::mutex> lock(activeConnsMutex_);
           if (activeConns_ > 5)
           {
               emit result(false);
              // stopStress(true);
           }
           else
           {
               emit result(true);
           }
       }
   }
}

void ConnectionsStress::clientThread_(int conn_socket)
{
    int counter_ = 0;
    bool once = true;
    try
    {
        std::string str_counter = std::to_string(counter_);
        if (send(conn_socket, str_counter.c_str(), str_counter.size(), 0) < 0)
        {
            std::lock_guard<std::mutex> lock(activeConnsMutex_);
            --activeConns_;
            throw std::runtime_error("refused connection");
        }

        char buff[7];
        while (stressRunning_.load())
        {
            int bytes = recv(conn_socket, buff, 7, 0);

            if (once)
            {
                std::lock_guard<std::mutex> lock(activeConnsMutex_);
                ++activeConns_;
                once = false;
            }
            if (bytes <= 0)
            {
                std::lock_guard<std::mutex> lock(activeConnsMutex_);
                --activeConns_;
                break;
            }
            else
            {

                int value = atoi(buff);
                counter_ = value;
                if (send(conn_socket, buff, 7, 0) < 0)
                {
                    std::lock_guard<std::mutex> lock(activeConnsMutex_);
                    --activeConns_;
                    break;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        close(conn_socket);
    }
    catch(const std::exception& e)
    {
        close(conn_socket);
    }
}

ConnectionsStress::~ConnectionsStress()
{
    stopStress(true);
}


