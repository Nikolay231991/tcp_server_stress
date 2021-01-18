#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "tcpclient.h"
#include "connectionsstress.h"
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void counterMonitorThread();
private slots:
    void connect_1(bool);
    void connect_2(bool);
    void connect_3(bool);
    void connect_4(bool);
    void connect_5(bool);
    void connect_6(bool);
    void connect_7(bool);

    void disconnect_1(bool);
    void disconnect_2(bool);
    void disconnect_3(bool);
    void disconnect_4(bool);
    void disconnect_5(bool);
    void disconnect_6(bool);
    void disconnect_7(bool);

    void set_disconnect_1();
    void set_disconnect_2();
    void set_disconnect_3();
    void set_disconnect_4();
    void set_disconnect_5();
    void set_disconnect_6();
    void set_disconnect_7();

    void onCounterChange_1(int);
    void onCounterChange_2(int);
    void onCounterChange_3(int);
    void onCounterChange_4(int);
    void onCounterChange_5(int);
    void onCounterChange_6(int);
    void onCounterChange_7(int);

    void stressCounter(int);
    void stressResult(bool);
private:
    Ui::MainWindow *ui;

    std::vector<std::shared_ptr<TcpClient>> tcpClients_;
    std::atomic<bool> countMonitorRunning_;
    std::shared_ptr<std::thread> countMonitorThread_;
    std::shared_ptr<ConnectionsStress> connStress_;
};
#endif // MAINWINDOW_H
