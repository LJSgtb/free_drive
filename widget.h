#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QString>
#include <QThread>
#include<QLocalSocket>
#include<QMessageBox>
#include <QFile>
#include <QList>

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <math.h>


#include "DianaAPI.h"
#include "DianaAPIDef.h"
#include "Environment.h"

#include "six_axis_sensor.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void get_six_froce(int m);

    QByteArray QString2Hex(QString str);

    char ConvertHexChar(char ch);

    double froce[6][6] = {{0.0}};

    double tcppos[6][6] = {{0.0}};

    double calibrate_zero_offset[6] = {0.0};

    double WorkPiece_frame[6] = {0.0};

    int direct_free_drive(bool run, bool is_virtual_wall, int *direction);

    int run_six_force(bool open, bool compensate);

    bool stop_six_sensor_calibration = false;

    double f[6] = {0.0};

    double tool_width = 0.0;

private slots:
    void on_pushButton_2_clicked();

    void on_External_force_value_clicked();

    void on_enter_admittance_clicked();

    void on_run_six_force_clicked();

    void on_six_force_calibration_clicked();

    void on_calibrate_zero_clicked();


    void on_get_tcppos_clicked();

signals:
    void sig_receivedCommand(QString str);

public slots:
    void receive_data(double *froce);

private:
    Ui::Widget *ui;
    QTimer* timer;
    QLocalSocket *localsocket;//套接字

    Six_axis_sensor *myThread = nullptr;

    //重心
    double Center_of_gravity[3] = {0.0};
    //常数
    double K1K2K3[3] = {0.0};
    //传感器零偏数据
    double zero_offset[6] = {0.0};
    //常数
    double LxLyLz[3] = {0.0};
    //重量
    double G = 0;
    //机器人安装角
    double UV[2] = {0.0};




    void wait_move();

    int admittance(int *direction);

    bool Virtual_wall = false;

    bool Running = false;

    bool run_six_force_info = false;


    QVector<QVector3D> curve;//曲线

    bool isok_Security_boundary = false;

    bool robot_link_state = false;

    bool calibration_info = false;

    int is_error=0;


};
#endif // WIDGET_H
