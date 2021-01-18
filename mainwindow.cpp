#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , countMonitorRunning_(false)
    ,  countMonitorThread_(nullptr)
{
    ui->setupUi(this);

    ui->conn_status_1->setText("disconnected");
    ui->conn_status_2->setText("disconnected");
    ui->conn_status_3->setText("disconnected");
    ui->conn_status_4->setText("disconnected");
    ui->conn_status_5->setText("disconnected");
    ui->conn_status_6->setText("disconnected");
    ui->conn_status_7->setText("disconnected");

    tcpClients_.resize(7);

    QString ip_address = ui->server_IP->text();
    QString port_str = ui->server_PORT->text();
    std::string ip_serv_address;
    int tcp_port;
    if (!port_str.isEmpty()) {
        tcp_port = port_str.toInt();
    }
    else {
        tcp_port = 3425;
    }

    if (!ip_address.isEmpty()) {
        ip_serv_address = ip_address.toStdString();
    }
    else {
        ip_serv_address = "192.168.42.1";
    }


    for (auto& tcpClient: tcpClients_)
    {
        tcpClient = std::make_shared<TcpClient>(ip_serv_address, tcp_port);
    }

    /* Bind connect signals */
    connect(ui->btn_connect_1, SIGNAL(clicked(bool)), this, SLOT(connect_1(bool)));
    connect(ui->btn_connect_2, SIGNAL(clicked(bool)), this, SLOT(connect_2(bool)));
    connect(ui->btn_connect_3, SIGNAL(clicked(bool)), this, SLOT(connect_3(bool)));
    connect(ui->btn_connect_4, SIGNAL(clicked(bool)), this, SLOT(connect_4(bool)));
    connect(ui->btn_connect_5, SIGNAL(clicked(bool)), this, SLOT(connect_5(bool)));
    connect(ui->btn_connect_6, SIGNAL(clicked(bool)), this, SLOT(connect_6(bool)));
    connect(ui->btn_connect_7, SIGNAL(clicked(bool)), this, SLOT(connect_7(bool)));

    /* Bind disconnect signals */
    connect(ui->btn_disconnect_1, SIGNAL(clicked(bool)), this, SLOT(disconnect_1(bool)));
    connect(ui->btn_disconnect_2, SIGNAL(clicked(bool)), this, SLOT(disconnect_2(bool)));
    connect(ui->btn_disconnect_3, SIGNAL(clicked(bool)), this, SLOT(disconnect_3(bool)));
    connect(ui->btn_disconnect_4, SIGNAL(clicked(bool)), this, SLOT(disconnect_4(bool)));
    connect(ui->btn_disconnect_5, SIGNAL(clicked(bool)), this, SLOT(disconnect_5(bool)));
    connect(ui->btn_disconnect_6, SIGNAL(clicked(bool)), this, SLOT(disconnect_6(bool)));
    connect(ui->btn_disconnect_7, SIGNAL(clicked(bool)), this, SLOT(disconnect_7(bool)));

    connect(tcpClients_[0].get(), SIGNAL(changedCounter(int)), SLOT(onCounterChange_1(int)));
    connect(tcpClients_[1].get(), SIGNAL(changedCounter(int)), SLOT(onCounterChange_2(int)));
    connect(tcpClients_[2].get(), SIGNAL(changedCounter(int)), SLOT(onCounterChange_3(int)));
    connect(tcpClients_[3].get(), SIGNAL(changedCounter(int)), SLOT(onCounterChange_4(int)));
    connect(tcpClients_[4].get(), SIGNAL(changedCounter(int)), SLOT(onCounterChange_5(int)));
    connect(tcpClients_[5].get(), SIGNAL(changedCounter(int)), SLOT(onCounterChange_6(int)));
    connect(tcpClients_[6].get(), SIGNAL(changedCounter(int)), SLOT(onCounterChange_7(int)));

    connect(tcpClients_[0].get(), SIGNAL(disconnection()), SLOT(set_disconnect_1()));
    connect(tcpClients_[1].get(), SIGNAL(disconnection()), SLOT(set_disconnect_2()));
    connect(tcpClients_[2].get(), SIGNAL(disconnection()), SLOT(set_disconnect_3()));
    connect(tcpClients_[3].get(), SIGNAL(disconnection()), SLOT(set_disconnect_4()));
    connect(tcpClients_[4].get(), SIGNAL(disconnection()), SLOT(set_disconnect_5()));
    connect(tcpClients_[5].get(), SIGNAL(disconnection()), SLOT(set_disconnect_6()));
    connect(tcpClients_[6].get(), SIGNAL(disconnection()), SLOT(set_disconnect_7()));


    /* Connection stess initialization*/
    connStress_ = std::make_shared<ConnectionsStress>();

    connect(ui->start_conns_stress, SIGNAL(clicked(bool)), connStress_.get(), SLOT(makeStress(bool)));
    connect(ui->stop_conns_stress, SIGNAL(clicked(bool)), connStress_.get(), SLOT(stopStress(bool)));

    connect(connStress_.get(), SIGNAL(connectionsCounter(int)), this, SLOT(stressCounter(int)));
    connect(connStress_.get(), SIGNAL(result(bool)), this, SLOT(stressResult(bool)));

}

MainWindow::~MainWindow()
{
    if (countMonitorThread_ != nullptr)
    {
        countMonitorRunning_.store(false);
        if (countMonitorThread_->joinable())
        {
            countMonitorThread_->join();
        }

        countMonitorThread_.reset();
    }
    delete ui;
}

void MainWindow::counterMonitorThread()
{
    ui->conn_counter_1->setText(QString::number(0));
    ui->conn_counter_2->setText(QString::number(0));
    ui->conn_counter_3->setText(QString::number(0));
    ui->conn_counter_4->setText(QString::number(0));
    ui->conn_counter_5->setText(QString::number(0));
    ui->conn_counter_6->setText(QString::number(0));
    ui->conn_counter_7->setText(QString::number(0));

    std::map<int, int> counters;
    while (countMonitorRunning_)
    {
        for (int i =0; i < 7; ++i)
        {
            if (tcpClients_[i]->isConnected())
            {
                int counter = tcpClients_[i]->getCounter();

                switch (i)
                {
                    case 0:
                        ui->conn_counter_1->setText(QString::number(counter));
                        if (((counter - counters[i]) != 1) && ((counter - counters[i]) != 0))
                        {
                            ui->log_info_1->setText("wrong counter number");
                        }
                        counters[i] = counter;
                    break;

                    case 1:
                        ui->conn_counter_2->setText(QString::number(counter));
                        if (((counter - counters[i]) != 1) && ((counter - counters[i]) != 0))
                        {
                            ui->log_info_2->setText("wrong counter number");
                        }
                        counters[i] = counter;
                    break;

                    case 2:
                        ui->conn_counter_3->setText(QString::number(counter));
                        if (((counter - counters[i]) != 1) && ((counter - counters[i]) != 0))
                        {
                            ui->log_info_3->setText("wrong counter number");
                        }
                        counters[i] = counter;
                    break;

                    case 3:
                        ui->conn_counter_4->setText(QString::number(counter));
                        if (((counter - counters[i]) != 1) && ((counter - counters[i]) != 0))
                        {
                            ui->log_info_4->setText("wrong counter number");
                        }
                        counters[i] = counter;
                    break;

                    case 4:
                        ui->conn_counter_5->setText(QString::number(counter));
                        if (((counter - counters[i]) != 1) && ((counter - counters[i]) != 0))
                        {
                            ui->log_info_5->setText("wrong counter number");
                        }
                        counters[i] = counter;
                    break;

                    case 5:
                        ui->conn_counter_6->setText(QString::number(counter));
                        if (((counter - counters[i]) != 1) && ((counter - counters[i]) != 0))
                        {
                            ui->log_info_6->setText("wrong counter number");
                        }
                        counters[i] = counter;
                    break;

                    case 6:
                        ui->conn_counter_7->setText(QString::number(counter));
                        if (((counter - counters[i]) != 1) && ((counter - counters[i]) != 0))
                        {
                            ui->log_info_7->setText("wrong counter number");
                        }
                        counters[i] = counter;
                    break;
                }
            }
        }

         std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void MainWindow::connect_1(bool)
{
    try
    {
        ui->conn_status_1->setText("wait for connection");
        tcpClients_[0]->tcp_connect();
        ui->conn_status_1->setText("connected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_1->setText(e.what());
    }
}

void MainWindow::connect_2(bool)
{
    try
    {
        ui->conn_status_2->setText("wait for connection");
        tcpClients_[1]->tcp_connect();
        ui->conn_status_2->setText("connected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_2->setText(e.what());
    }
}

void MainWindow::connect_3(bool)
{
    try
    {
        ui->conn_status_3->setText("wait for connection");
        tcpClients_[2]->tcp_connect();
        ui->conn_status_3->setText("connected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_3->setText(e.what());
    }
}

void MainWindow::connect_4(bool)
{
    try
    {
        ui->conn_status_4->setText("wait for connection");
        tcpClients_[3]->tcp_connect();
        ui->conn_status_4->setText("connected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_4->setText(e.what());
    }
}

void MainWindow::connect_5(bool)
{
    try
    {
        ui->conn_status_5->setText("wait for connection");
        tcpClients_[4]->tcp_connect();
        ui->conn_status_5->setText("connected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_5->setText(e.what());
    }
}

void MainWindow::connect_6(bool)
{
    try
    {
        ui->conn_status_6->setText("wait for connection");
        tcpClients_[5]->tcp_connect();
        ui->conn_status_6->setText("connected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_6->setText(e.what());
    }
}

void MainWindow::connect_7(bool)
{
    try
    {
        ui->conn_status_7->setText("wait for connection");
        tcpClients_[6]->tcp_connect();
        ui->conn_status_7->setText("connected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_7->setText(e.what());
    }
}

void MainWindow::disconnect_1(bool)
{
    try
    {
        tcpClients_[0]->disconnect();
        ui->conn_status_1->setText("disconnected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_1->setText(e.what());
    }
}

void MainWindow::disconnect_2(bool)
{
    try
    {
        tcpClients_[1]->disconnect();
        ui->conn_status_2->setText("disconnected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_2->setText(e.what());
    }
}

void MainWindow::disconnect_3(bool)
{
    try
    {
        tcpClients_[2]->disconnect();
        ui->conn_status_3->setText("disconnected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_3->setText(e.what());
    }
}

void MainWindow::disconnect_4(bool)
{
    try
    {
        tcpClients_[3]->disconnect();
        ui->conn_status_4->setText("disconnected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_4->setText(e.what());
    }
}

void MainWindow::disconnect_5(bool)
{
    try
    {
        tcpClients_[4]->disconnect();
        ui->conn_status_5->setText("disconnected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_5->setText(e.what());
    }
}

void MainWindow::disconnect_6(bool)
{
    try
    {
        tcpClients_[5]->disconnect();
        ui->conn_status_6->setText("disconnected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_6->setText(e.what());
    }
}

void MainWindow::disconnect_7(bool)
{
    try
    {
        tcpClients_[6]->disconnect();
        ui->conn_status_7->setText("disconnected");
    }
    catch (const std::exception& e)
    {
        ui->log_info_7->setText(e.what());
    }
}

void MainWindow::set_disconnect_1()
{
    disconnect_1(true);
}

void MainWindow::set_disconnect_2()
{
    disconnect_2(true);
}

void MainWindow::set_disconnect_3()
{
    disconnect_3(true);
}

void MainWindow::set_disconnect_4()
{
    disconnect_4(true);
}

void MainWindow::set_disconnect_5()
{
    disconnect_5(true);
}

void MainWindow::set_disconnect_6()
{
    disconnect_6(true);
}

void MainWindow::set_disconnect_7()
{
    disconnect_7(true);
}

void MainWindow::onCounterChange_1(int counter)
{
    static int  prevCounter = 0;
    ui->conn_counter_1->setText(QString::number(counter));
    if ((counter - prevCounter) != 1)
    {
        ui->log_info_1->setText("wrong counter number");
    }
    prevCounter = counter;
}

void MainWindow::onCounterChange_2(int counter)
{
    static int  prevCounter = 0;
    ui->conn_counter_2->setText(QString::number(counter));
    if ((counter - prevCounter) != 1)
    {
        ui->log_info_2->setText("wrong counter number");
    }
    prevCounter = counter;
}

void MainWindow::onCounterChange_3(int counter)
{
    static int  prevCounter = 0;
    ui->conn_counter_3->setText(QString::number(counter));
    if ((counter - prevCounter) != 1)
    {
        ui->log_info_3->setText("wrong counter number");
    }
    prevCounter = counter;
}

void MainWindow::onCounterChange_4(int counter)
{
    static int  prevCounter = 0;
    ui->conn_counter_4->setText(QString::number(counter));
    if ((counter - prevCounter) != 1)
    {
        ui->log_info_4->setText("wrong counter number");
    }
    prevCounter = counter;
}

void MainWindow::onCounterChange_5(int counter)
{
    static int  prevCounter = 0;
    ui->conn_counter_5->setText(QString::number(counter));
    if ((counter - prevCounter) != 1)
    {
        ui->log_info_5->setText("wrong counter number");
    }
    prevCounter = counter;
}

void MainWindow::onCounterChange_6(int counter)
{
    static int  prevCounter = 0;
    ui->conn_counter_6->setText(QString::number(counter));
    if ((counter - prevCounter) != 1)
    {
        ui->log_info_6->setText("wrong counter number");
    }
    prevCounter = counter;
}

void MainWindow::onCounterChange_7(int counter)
{
    static int  prevCounter = 0;
    ui->conn_counter_7->setText(QString::number(counter));
    if ((counter - prevCounter) != 1)
    {
        ui->log_info_7->setText("wrong counter number");
    }
    prevCounter = counter;
}

void MainWindow::stressCounter(int counter)
{
    if (counter > 0)
    {
        ui->active_conns->setText(QString::number(counter));
    }
    else
    {
        ui->active_conns->setText("0");
    }
}

void MainWindow::stressResult(bool result)
{
    if (result == true)
    {
        ui->conns_stress_result->setText("SUCCESS");
    }
    else
    {
        ui->conns_stress_result->setText("FAIL");
    }
}



