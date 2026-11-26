//#ifndef MYTHREAD_H
#ifndef SIX_AXIS_SENSOR_H
#define SIX_AXIS_SENSOR_H
//#define MYTHREAD_H

#include <QObject>
#include <QThread>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QUdpSocket>
#include <QNetworkDatagram>


#include <fstream>
#include <iostream>
#include <math.h>

#include "DianaAPI.h"
#include "DianaAPIDef.h"
#include "Environment.h"


using namespace std;

class Six_axis_sensor : public QThread
{
    Q_OBJECT

public:
    explicit Six_axis_sensor(QObject *parent = nullptr);
     ~Six_axis_sensor();

    void run();

    void stop();

    bool isok_sub;

    bool is_compensate = true;

//    QUdpSocket *mSocket_sub;

signals:
    void signal_message(double *f);

private:


    //重心
    double Center_of_gravity_sub[3] = {0.0};
    //常数
    double K1K2K3_sub[3] = {0.0};
    //传感器零偏数据
    double zero_offset_sub[6] = {0.0};
    //常数
    double LxLyLz_sub[3] = {0.0};
    //重量
    double G_sub = 0;
    //机器人安装角
    double UV_sub[2] = {0.0};


};

#endif // MYTHREAD_H
