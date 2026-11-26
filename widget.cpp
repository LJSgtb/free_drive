#include "widget.h"
#include "ui_widget.h"
#include <QVector>
#include <QTimer>
#include <QVector3D>
#include <QtConcurrent>

#include <windows.h>

bool isok = false, is_rec = false;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
//    this->move(1900,0);

    localsocket=new QLocalSocket(this);

    connect(localsocket,&QLocalSocket::readyRead,[=]()
    {
        QByteArray block;
        block=localsocket->readAll();//读取信息
        ui->textEdit->append(QString(u8"服务器：%1").arg(block.data()));
        QList<QByteArray> strlist = block.split(',');
        if(strlist[0] == "six_calibration")
        {
            if(strlist[1] == "1")
            {
                is_rec = true;
                on_six_force_calibration_clicked();
            }
            else
            {
                stop_six_sensor_calibration = true;
                run_six_force(false,false);
                if(localsocket->isValid())
                {
                    localsocket->write("0");
                    ui->textEdit->append(QString(u8"客户端：%1").arg("0"));
                }
            }
        }
        else if(strlist[0] == "calibrate_zero")
        {
            is_rec = true;
            on_calibrate_zero_clicked();
        }
        else if(strlist[0] == "set_Security_boundary")
        {
            if(!Running)
            {
                curve.clear();
                QVector3D point;
                if((strlist[1].toInt() >= 6) && (strlist[1].toInt()%3 == 0))
                {
                    for(int i = 0; i < strlist[1].toInt()/3; i++)
                    {
                        point.setX(strlist[i*3+2].toFloat());
                        point.setY(strlist[i*3+3].toFloat());
                        point.setZ(strlist[i*3+4].toFloat());
                        curve.push_back(point);
                    }
                    tool_width = strlist[strlist[1].toInt() + 2].toDouble();
                    isok_Security_boundary = true;
                    if(localsocket->isValid())
                    {
                        localsocket->write("0");
                        ui->textEdit->append(QString(u8"客户端：%1").arg("0"));
                    }
                }
                else
                {
                    isok_Security_boundary = false;
                    if(localsocket->isValid())
                    {
                        localsocket->write("-121");
                        ui->textEdit->append(QString(u8"客户端：%1").arg("-121"));
                    }
                }
            }
            else
            {
                if(localsocket->isValid())
                {
                    localsocket->write("-120");
                    ui->textEdit->append(QString(u8"客户端：%1").arg("-120"));
                }
            }
        }
        else if(strlist[0] == "direct_free_drive")
        {
            is_rec = true;
            int dir[6] = {0};
            for (int i = 0; i < 6; i++)
            {
                dir[i] = strlist[i+3].toInt();
            }
            direct_free_drive(strlist[1].toInt(), strlist[2].toInt(), dir);
        }
        else if(strlist[0] == "run_six_force")
        {
            int ret = run_six_force(strlist[1].toInt(), strlist[2].toInt());

            if(ret == 0)
            {
                if(localsocket->isValid())
                {
                    localsocket->write("0");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(0));
                }
            }
            else
            {
                if(ret > 98)
                {
                    qDebug() << "six_sensor_disconnect";
                    if(localsocket->isValid())
                    {
                        localsocket->write("99");
                        ui->textEdit->append(QString(u8"客户端：%1").arg(99));
                    }
                }
                else if(ret > 97)
                {
                    qDebug() << "six_sensor Data remains unchanged";
                    if(localsocket->isValid())
                    {
                        localsocket->write("98");
                        ui->textEdit->append(QString(u8"客户端：%1").arg(98));
                    }
                }
                else if(ret > 96)
                {
                    qDebug() << "six_sensor_get_tcppos_fail";
                    if(localsocket->isValid())
                    {
                        localsocket->write("97");
                        ui->textEdit->append(QString(u8"客户端：%1").arg(97));
                    }
                }
                else if(ret > 95)
                {
                    qDebug() << "six_sensor_frame conversion_fail";
                    if(localsocket->isValid())
                    {
                        localsocket->write("96");
                        ui->textEdit->append(QString(u8"客户端：%1").arg(96));
                    }
                }
                else
                {
                    if(localsocket->isValid())
                    {
                        localsocket->write("-1");
                        ui->textEdit->append(QString(u8"客户端：%1").arg(-1));
                    }
                }
            }
        }
        else if(strlist[0] == "get_External_force")
        {
            QString str = "f";
            for(int i = 0; i < 6; i++)
            {
                str = str + "," + QString::number(f[i]);
            }
            if(localsocket->isValid())
            {
                localsocket->write(str.toUtf8());
                ui->textEdit->append(QString(u8"客户端：%1").arg(str));
            }
        }
        else if(strlist[0] == "get_Calibration_results")
        {
            QString str = "r";
            for(int i = 0; i < 3; i++)
            {
                str = str + "," + QString::number(Center_of_gravity[i]);
            }
            for(int i = 0; i < 6; i++)
            {
                str = str + "," + QString::number(zero_offset[i]);
            }
            str = str + "," + QString::number(G);
            if(localsocket->isValid())
            {
                localsocket->write(str.toUtf8());
                ui->textEdit->append(QString(u8"客户端：%1").arg(str));
            }
        }
        else if(strlist[0] == "set_WorkPiece_frame")
        {
            for(int i = 0; i < 6; i++)
            {
                WorkPiece_frame[i] = strlist[i+1].toDouble();
            }
            if(localsocket->isValid())
            {
                localsocket->write("0");
                ui->textEdit->append(QString(u8"客户端：%1").arg(0));
            }
        }
        else if(strlist[0] == "hide")
        {
//            this->hide();
            this->showMinimized();
//            if(localsocket->isValid())
//            {
//                localsocket->write("0");
//                ui->textEdit->append(QString(u8"客户端：%1").arg(0));
//            }
        }
        else if(strlist[0] == "display")
        {
            this->show();
//            if(localsocket->isValid())
//            {
//                localsocket->write("0");
//                ui->textEdit->append(QString(u8"客户端：%1").arg(0));
//            }
        }
        else if(strlist[0] == "connect_robot")
        {
            srv_net_st * pinfo = new srv_net_st();
            memset(pinfo ->SrvIp, 0x00, sizeof(pinfo ->SrvIp));
            memcpy(pinfo ->SrvIp, "192.168.10.75", strlen("192.168.10.75"));
            pinfo->LocHeartbeatPort = 0;
            pinfo->LocRobotStatePort = 0;
            pinfo->LocSrvPort = 0;
            int ret = initSrv(nullptr, nullptr, pinfo);
            if(pinfo)
            {
             delete pinfo;
             pinfo = nullptr;
            }
            releaseBrake();
            if(ret < 0)
            {
                if(localsocket->isValid())
                {
                    localsocket->write("-1");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(-1));
                }
                robot_link_state = false;
            }
            else
            {
                if(localsocket->isValid())
                {
                    localsocket->write("0");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(0));
                }
                robot_link_state = true;
            }
        }
        else if(strlist[0] == "get_robot_link_state")
        {
            if(robot_link_state)
            {
                if(localsocket->isValid())
                {
                    localsocket->write("0");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(0));
                }
            }
            else
            {
                if(localsocket->isValid())
                {
                    localsocket->write("-1");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(-1));
                }
            }
        }
        else if(strlist[0] == "get_is_six_calibration")
        {
            if(calibration_info)
            {
                if(localsocket->isValid())
                {
                    localsocket->write("0");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(0));
                }
            }
            else
            {
                if(localsocket->isValid())
                {
                    localsocket->write("-1");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(-1));
                }
            }
        }
        else if(strlist[0] == "get_error")
        {
            if(localsocket->isValid())
            {
                localsocket->write(QString::number(is_error).toUtf8());
                ui->textEdit->append(QString(u8"客户端：%1").arg(is_error));
            }
        }
        else
        {
            qDebug() << "receive_error";
            if(localsocket->isValid())
            {
                localsocket->write("-1");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-1));
            }
        }

    });

    localsocket->abort();//先断开
    localsocket->connectToServer("agile robots");//连接

    if(!localsocket->waitForConnected(2000))
    {
        QMessageBox::information(this,"warning", u8"主进程连接失败,点击OK退出");
        localsocket = nullptr;
        delete localsocket;
        exit(0);
    }


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        ui->lcdNumber->display(round(f[0]*10)/10);
        ui->lcdNumber_2->display(round(f[1]*10)/10);
        ui->lcdNumber_3->display(round(f[2]*10)/10);
        ui->lcdNumber_4->display(round(f[3]*10)/10);
        ui->lcdNumber_5->display(round(f[4]*10)/10);
        ui->lcdNumber_6->display(round(f[5]*10)/10);
    });


    QString path = QCoreApplication::applicationDirPath() + "/reslut.txt";
    QFile file(path);
    QVector<QVector<double>> ff;
    QVector<double> f;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString line;

        while (!in.atEnd())
        {
            line = in.readLine(); // 读取第lineNumber行
            QStringList items = line.split(" ");
            foreach(QString item, items)
            {
                f.push_back(item.toDouble());
            }
            ff.push_back(f);
            f.clear();
        }
        file.close();
    }
    else
    {
        calibration_info = false;
    }

//    qDebug() << "----------------------------------";

    if(ff.length() == 6)
    {
        Center_of_gravity[0] = ff[0][0]; Center_of_gravity[1] = ff[0][1]; Center_of_gravity[2] = ff[0][2];
        K1K2K3[0] = ff[1][0]; K1K2K3[1] = ff[1][1]; K1K2K3[2] = ff[1][2];
        zero_offset[0] = ff[2][0]; zero_offset[1] = ff[2][1]; zero_offset[2] = ff[2][2];
        zero_offset[3] = ff[2][3]; zero_offset[4] = ff[2][4]; zero_offset[5] = ff[2][5];
        LxLyLz[0] = ff[3][0]; LxLyLz[1] = ff[3][1]; LxLyLz[2] = ff[3][2];
        G = ff[4][0];
        UV[0] = ff[5][0]; UV[1] = ff[5][1];
        calibration_info = true;
    }
    else
    {
        qDebug() << "read_file fail";
        calibration_info = false;
    }

    if(localsocket->isValid())
    {
        localsocket->write("0");
        ui->textEdit->append(QString(u8"客户端：%1").arg(0));
    }

    //连接机器人
    srv_net_st * pinfo = new srv_net_st();
    memset(pinfo ->SrvIp, 0x00, sizeof(pinfo ->SrvIp));
    memcpy(pinfo ->SrvIp, "192.168.10.75", strlen("192.168.10.75"));
    pinfo->LocHeartbeatPort = 0;
    pinfo->LocRobotStatePort = 0;
    pinfo->LocSrvPort = 0;
    int ret = initSrv(nullptr, nullptr, pinfo);
    if(pinfo)
    {
     delete pinfo;
     pinfo = nullptr;
    }
    if(ret < 0)
    {
        robot_link_state = false;
//        QMessageBox::information(this,"warning", u8"机器人连接失败");
//        exit(0);
    }
    else
    {
        robot_link_state = true;
        releaseBrake();
    }

}

Widget::~Widget()
{
    if(timer)
    {
        if(timer->isActive())
        {
            timer->stop();
        }
        timer = nullptr;
        delete timer;
    }
    if(localsocket)
    {
        localsocket->close();
        localsocket = nullptr;
        delete localsocket;
    }
    if(myThread)
    {
        if(myThread->isRunning())
        {
            myThread->requestInterruption();
            myThread->wait();
        }
        myThread = nullptr;
        delete myThread;
    }
    delete ui;
}

//求行列式
double det(int n, double *aa)//n为阶数，aa为矩阵,求行列式
{
    if (n == 1)
        return aa[0];
    double *bb = new double[(n - 1)*(n - 1)];//创建n-1阶的代数余子式阵bb
    int mov = 0;//判断行是否移动
    double sum = 0.0;//sum为行列式的值
    for (int arow = 0; arow<n; arow++) // a的行数把矩阵a(nn)赋值到b(n-1)
    {
        for (int brow = 0; brow<n - 1; brow++)//把aa阵第一列各元素的代数余子式存到bb
        {
            mov = arow > brow ? 0 : 1; //bb中小于arow的行，同行赋值，等于的错过，大于的加一
            for (int j = 0; j<n - 1; j++)  //从aa的第二列赋值到第n列
            {
                bb[brow*(n - 1) + j] = aa[(brow + mov)*n + j + 1];
            }
        }
        int flag = (arow % 2 == 0 ? 1: -1);//因为列数为0，所以行数是偶数时候，代数余子式为1.
        sum += flag* aa[arow*n] * det(n - 1, bb);//aa第一列各元素与其代数余子式积的和即为行列式
    }
    delete[]bb;
    return sum;
}
//矩阵加法
template<size_t arows, size_t acols, size_t brows, size_t bcols, size_t ret_row, size_t ret_bcol>
void matrix_plus(double (&a)[arows][acols], double (&b)[brows][bcols],  double (&ret)[ret_row][ret_bcol])
{
    if(acols != bcols || arows != brows)
    {
        qDebug() << "矩阵无法相加";
        return;
    }

    for(int i = 0; i < arows; i++)
    {
        for(int j = 0; j < acols; j++)
        {
            ret[i][j] = a[i][j] + b[i][j];
        }
    }
}
//矩阵减法
template<size_t arows, size_t acols, size_t brows, size_t bcols, size_t ret_row, size_t ret_bcol>
void matrix_reduce(double (&a)[arows][acols], double (&b)[brows][bcols],  double (&ret)[ret_row][ret_bcol])
{
    if(acols != bcols || arows != brows)
    {
        qDebug() << "矩阵无法相减";
        return;
    }

    for(int i = 0; i < arows; i++)
    {
        for(int j = 0; j < acols; j++)
        {
            ret[i][j] = a[i][j] - b[i][j];
        }
    }
}
//矩阵乘法
template<size_t arows, size_t acols, size_t brows, size_t bcols, size_t ret_row, size_t ret_bcol>
void matrix_multiplication(double (&a)[arows][acols], double (&b)[brows][bcols],  double (&ret)[ret_row][ret_bcol])
{
    if(acols != brows)
    {
        qDebug() << "矩阵无法相乘";
        return;
    }
    for(int i = 0; i < arows; i++)
    {
        for(int j = 0; j < bcols; j++)
        {
            double t = 0;
            for(int k = 0; k < acols; k++)
            {
                t = t + (a[i][k] * b[k][j]);
            }
            ret[i][j] = t;
        }
    }
}
//矩阵转置
template<size_t arows, size_t acols, size_t ret_row, size_t ret_bcol>
void matrix_transpose(double (&a)[arows][acols],  double (&ret)[ret_row][ret_bcol])
{
    for(int i = 0; i < acols; i++)
    {
        for(int j = 0; j < arows; j++)
        {
            ret[i][j] = a[j][i];
        }
    }
}
//求逆矩阵
template<size_t arows, size_t acols, size_t ret_row, size_t ret_bcol>
void matrix_inverse(double (&a)[arows][acols],  double (&ret)[ret_row][ret_bcol])
{
    if(acols==1 && arows==1)
    {
        ret[0][0] = 1/a[0][0];
        return;
    }
    int i,j,k,l,m,n;
//    double temp,temp2;
    double ret1[ret_row][ret_bcol];
    double temp[acols*arows];
    double temp2[(acols-1)*(arows-1)];
    QVector<double> var;
    var.clear();
//    qDebug() << "var内有多少元素：" << var.length();

    //求代数余子式
//    qDebug() << "rows:" << rows;
    for (i = 0; i < arows; i++)
    {
        for (j = 0; j < arows; j++)
        {
            for (k = 0; k < arows; k++)
            {
                for (l = 0; l < arows; l++)
                {
                    if(k != i && l != j)
                    {
//                        qDebug() << "A的值" << A[k][l];
                        var.append(a[k][l]);
                    }
                }
            }
//            qDebug() << "var内有多少元素：" << var.length();
            for(m = 0; m < var.length(); m++)
            {
                temp2[m] = var[m];
            }
            var.clear();
//            qDebug() << "det(cols-1,temp2):" << det(cols-1,temp2);
            ret[i][j] = pow(-1,i+j)*det(acols-1,temp2);
        }
    }
    //矩阵转置
    matrix_transpose(ret, ret1);
    for(int i = 0; i < ret_row; i++)
    {
        for(int j = 0; j < ret_bcol; j++)
        {
            ret[i][j] = ret1[i][j];
        }
    }
    //求逆矩阵
    //求A的行列式
    for(m = 0; m < arows; m++)
    {
        for(n = 0; n < acols; n++)
        {
            temp[m*acols+n] = a[m][n];
        }
    }
//    qDebug() << "det(cols,temp):" << det(cols,temp);
    double determinant = det(acols,temp);
    for(m = 0; m < arows; m++)
    {
        for(n = 0; n < acols; n++)
        {
            ret[m][n] = ret[m][n]/determinant;
        }
    }

//    qDebug() << res;
//    qDebug() << "__________________________________________";

}

//判断点P在多边形内-射线法
bool InsidePolygon(QVector<QPointF> polygon, QPointF pt)
{
    int i,j;
    bool inside,redo;

    int N = polygon.size();
    redo = true;
    for (i = 0;i < N;++i)
    {
        // 是否在顶点上
        if (polygon[i].x() == pt.x() && polygon[i].y() == pt.y())
        {
            redo = false;
            inside = true;
            break;
        }
    }

    while (redo)
    {
        redo = false;
        inside = false;
        for (i = 0,j = N - 1;i < N;j = i++)
        {
            if ( (polygon[i].y() < pt.y() && pt.y() < polygon[j].y()) ||
                (polygon[j].y() < pt.y() && pt.y() < polygon[i].y()) )
            {
                if (pt.x() <= polygon[i].x() || pt.x() <= polygon[j].x())
                {
                    double _x = (pt.y()-polygon[i].y())*(polygon[j].x()-polygon[i].x())/(polygon[j].y()-polygon[i].y())+polygon[i].x();
                    if (pt.x() < _x)          // 在线的左侧
                        inside = !inside;
                    else if (pt.x() == _x)    // 在线上
                    {
                        inside = true;
                        break;
                    }
                }
            }
            else if ( pt.y() == polygon[i].y())
            {
                if (pt.x() < polygon[i].x())    // 交点在顶点上
                {
                    if (polygon[i].y() > polygon[j].y())
                        pt.setY(pt.y() - 1);
                    else
                        pt.setY(pt.y() + 1);
                    redo = true;
                    break;
                }
            }
            else if ( polygon[i].y() ==  polygon[j].y() && pt.y() == polygon[i].y() &&
                ((polygon[i].x() < pt.x() && pt.x() < polygon[j].x()) ||
                (polygon[j].x() < pt.x() && pt.x() < polygon[i].x())) )// 在水平的边界线上
            {
                inside = true;
                break;
            }
        }
    }

    return inside;
}

void coordinate_transformation(double *srcPose, double *srcMatrixPose,double *dstMatrixPose,double *dstPose)
{
    double T[4][4];    double T1[4][4];    double T2[4][4];    double T3[4][4];    double ttt[16];

    pose2Homogeneous(srcPose, ttt);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            T[i][j] = ttt[(i*4)+j];
        }
    }
    matrix_transpose(T, T1);

    pose2Homogeneous(srcMatrixPose, ttt);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            T[i][j] = ttt[(i*4)+j];
        }
    }
    matrix_transpose(T, T2);

    pose2Homogeneous(dstMatrixPose, ttt);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            T[i][j] = ttt[(i*4)+j];
        }
    }
    matrix_transpose(T, T3);

    matrix_inverse(T3,T);
    matrix_multiplication(T, T2, T3);
    matrix_multiplication(T3, T1, T2);
    matrix_transpose(T2, T1);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ttt[(i*4)+j] = T1[i][j];
        }
    }
    homogeneous2Pose(ttt, dstPose);
}

void Widget::wait_move()
{
    QThread::msleep(20);
    while (true)
    {
        const char state = getRobotState();
        if (state != 0 || stop_six_sensor_calibration)
        {
            break;
        }
        else
        {
            QThread::msleep(1);
            QApplication::processEvents();
        }
    }
    stop();
}

int get_six_sensor_pos(double *joints, double *pose)
{
//    QVector<QVector<QVector<double>>> t;
    vector<double[4][4]> t(7);
    double matirx_tmp[4][4];
    double matirx_tmp1[4][4];
    QVector<double> tmp;


    double six_sensor_to_flange[4][4] = {{0,  1,  0,  0},
                                         {1,  0,  0,  0},
                                         {0,  0, -1,  0},
                                         {0,  0,  0,  1}};

    double ai[7] = {0.0},alph[7] = {0.0},di[7] = {0.0},theta[7] = {0.0};
    if(getDH(ai,alph,di,theta) != 0)
    {
        return -1;
    }

    for(int i = 0; i < 7; i++)
    {
        t[i][0][0]=cos(joints[i]+theta[i]);               t[i][0][1]=-sin(joints[i]+theta[i]);              t[i][0][2]=0;              t[i][0][3]=ai[i];
        t[i][1][0]=sin(joints[i]+theta[i])*cos(alph[i]);  t[i][1][1]=cos(joints[i]+theta[i])*cos(alph[i]);  t[i][1][2]=-sin(alph[i]);  t[i][1][3]=-di[i]*sin(alph[i]);
        t[i][2][0]=sin(joints[i]+theta[i])*sin(alph[i]);  t[i][2][1]=cos(joints[i]+theta[i])*sin(alph[i]);  t[i][2][2]=cos(alph[i]);   t[i][2][3]=di[i]*cos(alph[i]);
        t[i][3][0]=0;                                     t[i][3][1]=0;                                     t[i][3][2]=0;              t[i][3][3]=1;
    }

//    for(int j = 0; j < 6; j++)
//    {
//        matrix_multiplication(matirx_tmp, t[j+1], matirx_tmp);
//    }

    matrix_multiplication(t[0], t[1], matirx_tmp);
    matrix_multiplication(matirx_tmp, t[2], matirx_tmp1);
    matrix_multiplication(matirx_tmp1, t[3], matirx_tmp);
    matrix_multiplication(matirx_tmp, t[4], matirx_tmp1);
    matrix_multiplication(matirx_tmp1, t[5], matirx_tmp);
    matrix_multiplication(matirx_tmp, t[6], matirx_tmp1);
    matrix_multiplication(matirx_tmp1, six_sensor_to_flange, matirx_tmp);

    matrix_transpose(matirx_tmp, matirx_tmp1);


    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            tmp.push_back(matirx_tmp1[i][j]);
        }
    }
    homogeneous2Pose(tmp.data(), pose);

//    qDebug() << tmp;

//    AxisToRpy(pose);
//    qDebug() << pose[0]*1000 << "," << pose[1]*1000 << "," << pose[2]*1000 << "," << rad_to_deg(pose[3]) << "," << rad_to_deg(pose[4]) << "," << rad_to_deg(pose[5]);
    return 0;
}

void Widget::on_six_force_calibration_clicked()
{
    if(getLinkState())
    {
        robot_link_state = false;
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-100");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-100));
            is_rec = false;
        }
        calibration_info = false;
        return;
    }
    else
    {
        robot_link_state = true;
    }
    if(releaseBrake())
    {
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-101");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-101));
            is_rec = false;
        }
        calibration_info = false;
        return;
    }
    stop_six_sensor_calibration = false;
    if(!Running)
    {
        int ret = run_six_force(true, false);
        if(ret != 0)
        {
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("-102");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-102));
                is_rec = false;
            }
            return;
        }
        else
        {
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("0");
                ui->textEdit->append(QString(u8"客户端：%1").arg(0));
                is_rec = false;
            }
        }
        double joints[6][7] = {
            {0.183359 , -0.554174 , -0.0996127 , 1.99372 , 3.08879 , -1.44197 , -0.0918229},
            {0.181537 , -0.543604 , -0.106492 , 1.97452 , 1.50392 , -1.48885 , 0.137603},
            {0 , -0.515465 , 0 , 2.02548 , 0 , -1.6316 , 0},
            {0.0249753 , -0.533274 , -0.0236087 , 2.05367 , -1.58302 , -1.57485 , -0.0503096},
            {0 ,  -0.315389 ,  0 ,  2.39875 ,  0 ,  0.512565 ,  3.09929},
            {0 , -0.315377 , 0 , 2.39874 , 0 , 0.512566 , 0}};
        double vel = 0.4;
        double acc = 0.4;
        double joints_current[7] = {0.0};
        for(int i = 0; i < 6; i++)
        {
            moveJToTarget(joints[i], vel, acc);
            wait_move();
            if(stop_six_sensor_calibration)
            {
                calibration_info = false;
                return;
            }
            QThread::msleep(500);
            //获取六维力数据
            get_six_froce(i);
            if(getJointPos(joints_current) != 0)
            {
                qDebug() << "get tcppos fail";
//                if(localsocket->isValid()&&is_rec)
//                {
//                    localsocket->write("-102");
//                    ui->textEdit->append(QString(u8"客户端：%1").arg(-102));
//                    is_rec = false;
//                }
                calibration_info = false;
                return;
            }
            if(get_six_sensor_pos(joints_current, tcppos[i]) != 0)
            {
//                if(localsocket->isValid()&&is_rec)
//                {
//                    localsocket->write("-1021");
//                    ui->textEdit->append(QString(u8"客户端：%1").arg(-1021));
//                    is_rec = false;
//                }
                calibration_info = false;
                return;
            }
            QThread::msleep(500);
        }

        run_six_force(false, false);

        double f[18][6];
        double m[18][1];
        double t1[6][6];
        double t2[6][18];
        double t3[6][1];
        double t4[6][1];
        double tmp[6][18];
        double tmp1[6][6];

        for(int i = 0; i < 6; i++)
        {
            //力
            f[i*3][0] = 0;
            f[i*3][1] = froce[i][2];
            f[i*3][2] = -froce[i][1];
            f[i*3][3] = 1;
            f[i*3][4] = 0;
            f[i*3][5] = 0;

            f[i*3+1][0] = -froce[i][2];
            f[i*3+1][1] = 0;
            f[i*3+1][2] = froce[i][0];
            f[i*3+1][3] = 0;
            f[i*3+1][4] = 1;
            f[i*3+1][5] = 0;

            f[i*3+2][0] = froce[i][1];
            f[i*3+2][1] = -froce[i][0];
            f[i*3+2][2] = 0;
            f[i*3+2][3] = 0;
            f[i*3+2][4] = 0;
            f[i*3+2][5] = 1;

            //力矩
            for (int j = 0; j < 3; j++)
            {
                m[(i*3)+j][0] = froce[i][j+3];
            }
        }

        matrix_transpose(f, tmp);
        matrix_multiplication(tmp, f, t1);
        matrix_inverse(t1, tmp1);
        matrix_multiplication(tmp1, tmp, t2);
        matrix_multiplication(t2, m, t3);

        //打印结果
        qDebug() << "xyz,k1,k2,k3";
        Center_of_gravity[0] = t3[0][0];
        Center_of_gravity[1] = t3[1][0];
        Center_of_gravity[2] = t3[2][0];
        K1K2K3[0] = t3[3][0];
        K1K2K3[1] = t3[4][0];
        K1K2K3[2] = t3[5][0];
        qDebug() << Center_of_gravity[0] << " " << Center_of_gravity[1] << " " << Center_of_gravity[2] << " " <<
                    K1K2K3[0] << " " << K1K2K3[1] << " " <<K1K2K3[2];

        for(int i = 0; i < 6; i++)
        {
            double matrix[16]={0};
            pose2Homogeneous(tcppos[i], matrix);

            //姿态矩阵
            f[i*3][0] = matrix[0];
            f[i*3][1] = matrix[1];
            f[i*3][2] = matrix[2];
            f[i*3][3] = 1;
            f[i*3][4] = 0;
            f[i*3][5] = 0;

            f[i*3+1][0] = matrix[4];
            f[i*3+1][1] = matrix[5];
            f[i*3+1][2] = matrix[6];
            f[i*3+1][3] = 0;
            f[i*3+1][4] = 1;
            f[i*3+1][5] = 0;

            f[i*3+2][0] = matrix[8];
            f[i*3+2][1] = matrix[9];
            f[i*3+2][2] = matrix[10];
            f[i*3+2][3] = 0;
            f[i*3+2][4] = 0;
            f[i*3+2][5] = 1;

            //力
            for (int j = 0; j < 3; j++)
            {
                m[(i*3)+j][0] = froce[i][j];
            }
        }

        matrix_transpose(f, tmp);
        matrix_multiplication(tmp, f, t1);
        matrix_inverse(t1, tmp1);
        matrix_multiplication(tmp1, tmp, t2);
        matrix_multiplication(t2, m, t4);       //打印结果

        LxLyLz[0] = t4[0][0];
        LxLyLz[1] = t4[1][0];
        LxLyLz[2] = t4[2][0];
        zero_offset[0] = t4[3][0];
        zero_offset[1] = t4[4][0];
        zero_offset[2] = t4[5][0];

        zero_offset[3] = t3[3][0] - (t4[4][0]*t3[2][0]) + (t4[5][0]*t3[1][0]);
        zero_offset[4] = t3[4][0] - (t4[5][0]*t3[0][0]) + (t4[3][0]*t3[2][0]);
        zero_offset[5] = t3[5][0] - (t4[3][0]*t3[1][0]) + (t4[4][0]*t3[0][0]);

        G = sqrt(t4[0][0]*t4[0][0] + t4[1][0]*t4[1][0] + t4[2][0]*t4[2][0]);
        qDebug() << "G = " << G;

        UV[0] = asin(-t4[1][0] / G);
        UV[1] = atan(-t4[0][0] / t4[2][0]);

        QString path = QCoreApplication::applicationDirPath() + "/reslut.txt";
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.close();
        }

        ui->textEdit->append(QString(u8"G="));
        ui->textEdit->append(QString::number(G));
        if(G > 40 || G < 20)
        {
            ui->textEdit->append(QString(u8"标定失败cailb_fail"));
            calibration_info = false;
            return;
        }

        ofstream fout(path.toStdString());
        fout << Center_of_gravity[0] << " " << Center_of_gravity[1] << " " << Center_of_gravity[2]<<endl;//输出到txt文档
        fout << K1K2K3[0] << " " << K1K2K3[1] << " " << K1K2K3[2]<<endl;//输出到txt文档
        fout << zero_offset[0] << " " << zero_offset[1] << " " << zero_offset[2] << " "
                               << zero_offset[3] << " " << zero_offset[4] << " " << zero_offset[5] <<endl;//输出到txt文档
        fout << LxLyLz[0] << " " << LxLyLz[1] << " " << LxLyLz[2]<<endl;//输出到txt文档
        fout << G <<endl;//输出到txt文档
        fout << UV[0] << " " << UV[1] <<endl;//输出到txt文档

//        if(localsocket->isValid()&&is_rec)
//        {
//            localsocket->write("0");
//            ui->textEdit->append(QString(u8"客户端：%1").arg(0));
//            is_rec = false;
//        }
        calibration_info = true;
    }
    else
    {
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-103");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-103));
            is_rec = false;
        }
        calibration_info = false;
    }
}

void Widget::on_calibrate_zero_clicked()
{
    if(!Running)
    {
        if(!run_six_force_info)
        {
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("-111");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-111));
                is_rec = false;
            }
        }
        double force[6] = {0.0};

        for (int i = 0; i < 100; i++)
        {
            force[0] = force[0] + f[0];
            force[1] = force[1] + f[1];
            force[2] = force[2] + f[2];
            force[3] = force[3] + f[3];
            force[4] = force[4] + f[4];
            force[5] = force[5] + f[5];
            QThread::msleep(1);
            QApplication::processEvents();
        }

        for(int i = 0; i < 6; i++)
        {
            if(abs(force[i]/100) > 5 )
            {
                qDebug() << "too big error, calibrate_six_sensor_zero_offset fail";
                if(localsocket->isValid()&&is_rec)
                {
                    localsocket->write("-112");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(-112));
                    is_rec = false;
                }
                return;
            }
        }

        for(int i = 0; i < 6; i++)
        {
            calibrate_zero_offset[i] = force[i]/100;
        }

        ui->zero_x->setNum(calibrate_zero_offset[0]);
        ui->zero_y->setNum(calibrate_zero_offset[1]);
        ui->zero_z->setNum(calibrate_zero_offset[2]);
        ui->zero_rx->setNum(calibrate_zero_offset[3]);
        ui->zero_ry->setNum(calibrate_zero_offset[4]);
        ui->zero_rz->setNum(calibrate_zero_offset[5]);

        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("0");
            ui->textEdit->append(QString(u8"客户端：%1").arg(0));
            is_rec = false;
        }
    }
    else
    {
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-110");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-110));
            is_rec = false;
        }
    }
}

void Widget::on_pushButton_2_clicked()
{
    //连接机器人
    srv_net_st * pinfo = new srv_net_st();
    memset(pinfo ->SrvIp, 0x00, sizeof(pinfo ->SrvIp));
    memcpy(pinfo ->SrvIp, "192.168.10.75", strlen("192.168.10.75"));
    pinfo->LocHeartbeatPort = 0;
    pinfo->LocRobotStatePort = 0;
    pinfo->LocSrvPort = 0;
    int ret = initSrv(nullptr, nullptr, pinfo);
    if(ret < 0)
    {
        qDebug("192.168.10.75 initSrv failed! Return value = %d\n", ret);
    }
    if(pinfo)
    {
     delete pinfo;
     pinfo = nullptr;
    }
    releaseBrake();

}

void Widget::on_External_force_value_clicked()
{
    double matrix_E[3][3] = {{0.0}};
    double L[3][1] = {{0.0}};
    double GCxyz[3][1] = {{0.0}};

    L[0][0] = LxLyLz[0];
    L[1][0] = LxLyLz[1];
    L[2][0] = LxLyLz[2];


    double pos[6] = {0.0};
    double matrix[16]={0.0};
    getTcpPos(pos);
    pose2Homogeneous(pos, matrix);

    matrix_E[0][0] = matrix[0];matrix_E[0][1] = matrix[1];matrix_E[0][2] = matrix[2];
    matrix_E[1][0] = matrix[4];matrix_E[1][1] = matrix[5];matrix_E[1][2] = matrix[6];
    matrix_E[2][0] = matrix[8];matrix_E[2][1] = matrix[9];matrix_E[2][2] = matrix[10];


    //负载重力在传感器坐标系轴上的力分量
    matrix_multiplication(matrix_E, L, GCxyz);

    double GMx = (GCxyz[2][0] * Center_of_gravity[1]) - (GCxyz[1][0] * Center_of_gravity[2]);
    double GMy = (GCxyz[0][0] * Center_of_gravity[2]) - (GCxyz[2][0] * Center_of_gravity[0]);
    double GMz = (GCxyz[1][0] * Center_of_gravity[0]) - (GCxyz[0][0] * Center_of_gravity[1]);

    double Fw[6] = {0.0};
    Fw[0] = f[0] - zero_offset[0] - GCxyz[0][0];
    Fw[1] = f[1] - zero_offset[1] - GCxyz[1][0];
    Fw[2] = f[2] - zero_offset[2] - GCxyz[2][0];
    Fw[3] = f[3] - zero_offset[3] - GMx;
    Fw[4] = f[4] - zero_offset[4] - GMy;
    Fw[5] = f[5] - zero_offset[5] - GMz;

    QString result = QString::number(Fw[0]) + ",  " + QString::number(Fw[1]) + ",  " + QString::number(Fw[2]) + ",  "
            + QString::number(Fw[3]) + ",  " + QString::number(Fw[4]) + ",  " + QString::number(Fw[5]);

    ui->lineEdit->setText(result);

}

void Widget::on_enter_admittance_clicked()
{
    int direct[6] = {0};
    if(ui->enter_admittance->text() == QString::fromLocal8Bit("进入定向导纳") && ui->radioButton->isChecked() && myThread->isRunning())
    {
        ui->enter_admittance->setText(QString::fromLocal8Bit("退出定向导纳"));
        ui->run_six_force->setEnabled(false);
        isok = true;

        if(ui->X->isChecked()){direct[0] = 1;}
        if(ui->Y->isChecked()){direct[1] = 1;}
        if(ui->Z->isChecked()){direct[2] = 1;}
        if(ui->RX->isChecked()){direct[3] = 1;}
        if(ui->RY->isChecked()){direct[4] = 1;}
        if(ui->RZ->isChecked()){direct[5] = 1;}
        direct_free_drive(true, false, direct);
    }
    else
    {
        ui->enter_admittance->setText(QString::fromLocal8Bit("进入定向导纳"));
        ui->run_six_force->setEnabled(true);
        isok = false;
    }
}

void Widget::on_run_six_force_clicked()
{
    if(ui->run_six_force->text() == QString::fromLocal8Bit("开启六维力"))
    {
        ui->run_six_force->setText(QString::fromLocal8Bit("关闭六维力"));
        if(ui->radioButton->isChecked())
        {
            run_six_force(true,  true);
        }
        else
        {
            run_six_force(true,  false);
        }
        ui->calibrate_zero->setEnabled(true);
    }
    else
    {
        ui->run_six_force->setText(QString::fromLocal8Bit("开启六维力"));
        run_six_force(false,  false);
        ui->calibrate_zero->setEnabled(false);
    }
}

void Widget::get_six_froce(int m)
{
    double x_f = 0, y_f = 0, z_f = 0, x_m = 0, y_m = 0, z_m = 0;

    for (int i = 0; i < 500; i++)
    {
        x_f = x_f + f[0];
        y_f = y_f + f[1];
        z_f = z_f + f[2];
        x_m = x_m + f[3];
        y_m = y_m + f[4];
        z_m = z_m + f[5];
        QThread::msleep(2);
        QApplication::processEvents();
    }

    froce[m][0] = x_f/500;
    froce[m][1] = y_f/500;
    froce[m][2] = z_f/500;
    froce[m][3] = x_m/500;
    froce[m][4] = y_m/500;
    froce[m][5] = z_m/500;
    qDebug() << x_f/500 << ",  " << y_f/500 << ",  " << z_f/500 << ",  " << x_m/500 << ",  " << y_m/500 << ",  " << z_m/500;

}

void Widget::receive_data(double *froce)
{
    f[0] = froce[0];
    f[1] = froce[1];
    f[2] = froce[2];
    f[3] = froce[3];
    f[4] = froce[4];
    f[5] = froce[5];

    if(f[0] > 90)
    {
        is_error = f[0];
        qDebug() << "six_sensor_error";
        isok = false;
        Running = false;
        if(f[0] > 98)
        {
            qDebug() << "six_sensor_disconnect";
        }
        else if(f[0] > 97)
        {
            qDebug() << "six_sensor Data remains unchanged";
        }
        else if(f[0] > 96)
        {
            qDebug() << "six_sensor_get_tcppos_fail";
        }
        else if(f[0] > 95)
        {
            qDebug() << "six_sensor_frame conversion_fail";
        }
    }
//    QApplication::processEvents();
}


int Widget::admittance(int *direction)
{
    //确保六维力有数据
    for(int i = 0; i < 6; i++)
    {
        if(abs(f[i]) > 0)
        {
            break;
        }
        if(i == 5)
        {
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("-134");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-134));
                is_rec = false;
            }
            return -134;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        if(abs(f[i]-calibrate_zero_offset[i]) > 1)
        {
            qDebug() << "Threshold exceeded";
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("-135");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-135));
                is_rec = false;
            }
            return -135;
        }
    }
    for (int i = 3; i < 6; i++)
    {
        if(abs(f[i]-calibrate_zero_offset[i]) > 0.2)
        {
            qDebug() << "Threshold exceeded";
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("-135");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-135));
                is_rec = false;
            }
            return -135;
        }
    }

    //导纳参数，M代表惯性，B代表阻尼，K代表刚度
    double M=0.1,  B=25,  K=0, dt=0.0018;
    double xyze[6] = {0};                        //%x轴方向力引起的位移，速度，加速度增量初始值均为0
    double d0[6] = {0};
    double dxyze[6] = {0};
    double d1[6] = {0};
    double ddxyze[6] = {0};
    double d2[6] = {0};
    double force_ext[6] = {0.0};

    //获取当前位置
    int e = 0;//错误码
    double base[6] = {0.0};
    double dst_pose[6] = {0.0};
    double tmp_pose[6] = {0.0};
    double tmp_pose1[6] = {0.0};
    double tmp_pose2[6] = {0.0};
    double joints[7] = {0.0};
    double dst_joints[7] = {0.0};
    int ret = getTcpPos(dst_pose);
    if(ret != 0)
    {
        qDebug() << "gettcppos fail";
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-136");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-136));
            is_rec = false;
        }
        return -136;
    }
    ret = getJointPos(joints);
    if(ret != 0)
    {
        qDebug() << "getJointPos fail";
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-137");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-137));
            is_rec = false;
        }
        return -137;
    }
    memcpy(dst_joints, joints, 56);

    //实现随动的参数
    double error;
    double WorkPiece_pos[6] = {0.0};
    double p_pose[6] = {0.0};

    coordinate_transformation(dst_pose, base, WorkPiece_frame, tmp_pose);
    memcpy(p_pose, tmp_pose, 48);


    //虚拟墙参数
    bool inside = false;
//    int m = 0,n = 0;
    double offset_r[6] = {0.0};
    double offset_l[6] = {0.0};
    int v[6] = {0};

    memcpy(WorkPiece_pos, WorkPiece_frame, 48);

    if(!isok_Security_boundary&&Virtual_wall)
    {
        ui->lineEdit->setText("Virtual_wall error");
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-138");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-138));
            is_rec = false;
        }
        return -138;
    }

    int dir[3]={0}, t=0;
    for (int i = 0; i < 3; i++)
    {
        if(direction[i]!=0)
        {
            if(t>2)
            {
                if(localsocket->isValid()&&is_rec)
                {
                    localsocket->write("-139");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(-139));
                    is_rec = false;
                }
                return -139;
            }
            dir[t]=i;
            t++;
        }
    }
    if(t<1)
    {
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-140");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-140));
            is_rec = false;
        }
        return -140;
    }

    for (int i = 0; i < 3; i++)
    {
        if(dir[i] == 0)
        {
            if(abs(tmp_pose[i]) > 0.001)
            {
                ui->lineEdit->setText("plane error");
                if(localsocket->isValid()&&is_rec)
                {
                    localsocket->write("-143");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(-143));
                    is_rec = false;
                }
                return -143;
            }
        }
    }


    offset_l[dir[1]] = tool_width/2;
    offset_r[dir[1]] = -tool_width/2;
    QVector<QPointF> polygon;
    for (int i = 0; i < curve.size(); i++)
    {
        QPointF t(curve[i][dir[0]],curve[i][dir[1]]);
        polygon.append(t);
    }

    for(int i = 0; i < 6; i++)
    {
        if(direction[i]!=0)
        {
            coordinate_transformation(offset_l, tmp_pose, base, tmp_pose1);
            coordinate_transformation(offset_r, tmp_pose, base, tmp_pose2);
            QPointF p(tmp_pose1[dir[0]],tmp_pose1[dir[1]]);
            QPointF p1(tmp_pose[dir[0]],tmp_pose[dir[1]]);
            QPointF p2(tmp_pose2[dir[0]],tmp_pose2[dir[1]]);
            if(!InsidePolygon(polygon, p))
            {
                inside = false;
            }
            else if(!InsidePolygon(polygon, p1))
            {
                inside = false;
            }
            else if(!InsidePolygon(polygon, p2))
            {
                inside = false;
            }
            else
            {
                inside = true;
            }
            if(!inside)
            {
                ui->lineEdit->setText("Not inside the virtual wall");
                if(localsocket->isValid()&&is_rec)
                {
                    localsocket->write("-144");
                    ui->textEdit->append(QString(u8"客户端：%1").arg(-144));
                    is_rec = false;
                }
                return -144;
            }
        }
   }


    ui->calibrate_zero->setEnabled(false);

    if(localsocket->isValid()&&is_rec)
    {
        localsocket->write("0");
        ui->textEdit->append(QString(u8"客户端：%1").arg(0));
        is_rec = false;
    }
    Running = true;

    while(isok)
    {
        e = getLinkState();
        if(e != 0)
        {
            is_error = -141;
            isok = false;
            Running = false;
            stop();
//            if(localsocket->isValid()&&is_rec)
//            {
//                localsocket->write("-141");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-141));
//                is_rec = false;
//            }
           return -141;
        }
        e = getLastError();
        if(e != 0)
        {
            is_error = -142;
            isok = false;
            Running = false;
            stop();
//            if(localsocket->isValid()&&is_rec)
//            {
//                localsocket->write("-142");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-142));
//                is_rec = false;
//            }
            return -142;
        }

        for (int i = 0; i < 3; i++)
        {
            if(direction[i] != 0)
            {
                force_ext[i] = f[i]-calibrate_zero_offset[i];
                if(abs(force_ext[i]) < 4)
                {
                    force_ext[i]=0;
                }
                else
                {
                    if(force_ext[i] < 0)
                    {
                        force_ext[i]=force_ext[i] + 4;
                    }
                    else
                    {
                        force_ext[i]=force_ext[i] - 4;
                    }
                }
            }
            else
            {
                force_ext[i] = 0;
            }

        }

        for (int i = 3; i < 6; i++)
        {
            if(direction[i] != 0)
            {
                force_ext[i] = f[i]-calibrate_zero_offset[i];
                if(abs(force_ext[i]) < 0.2)
                {
                    force_ext[i]=0;
                }
            }
            else
            {
                force_ext[i] = 0;
            }

        }
        for (int i = 0; i < 6; i++)
        {
            if(force_ext[i] > 15){force_ext[i] = 15;} if(force_ext[i] < -15){force_ext[i] = -15;}
        }

        for (int i = 0; i < 3; i++)
        {
            error = WorkPiece_frame[i] - WorkPiece_pos[i];
            if(error > 0.00008)
            {WorkPiece_pos[i] += 0.00008;}
            else if(error < -0.00008)
            {WorkPiece_pos[i] -= 0.00008;}
            else {WorkPiece_pos[i] = WorkPiece_frame[i];}
        }
        for (int i = 3; i < 6; i++)
        {
            error = WorkPiece_frame[i] - WorkPiece_pos[i];
            if(error > 0.005)
            {WorkPiece_pos[i] += 0.005;}
            else if(error < -0.005)
            {WorkPiece_pos[i] -= 0.005;}
            else {WorkPiece_pos[i] = WorkPiece_frame[i];}
        }

        for (int i = 0; i < 3; i++)
        {
            ddxyze[i]=(force_ext[i]/3-B*d1[i]-K*d0[i])/M;
            dxyze[i]=(dt*(ddxyze[i]+d2[i])/2)+d1[i];
            xyze[i]=(dt*(dxyze[i]+d1[i])/2);
            d0[i] = xyze[i];
            d1[i] = dxyze[i];
            d2[i] = ddxyze[i];
        }
        for (int i = 3; i < 6; i++)
        {
            ddxyze[i]=(force_ext[i]*3-B*d1[i]-K*d0[i])/M;
            dxyze[i]=(dt*(ddxyze[i]+d2[i])/2)+d1[i];
            xyze[i]=(dt*(dxyze[i]+d1[i])/2);
            d0[i] = xyze[i];
            d1[i] = dxyze[i];
            d2[i] = ddxyze[i];
        }

        if(Virtual_wall)
        {
            double d[6]={0.0};

            for(int i = 0; i < 6; i++)
            {
                memcpy(d,tmp_pose,48);
                if(direction[i]!=0)
                {
                    d[i] = tmp_pose[i] + xyze[i];
                    coordinate_transformation(offset_l, d, base, tmp_pose1);
                    coordinate_transformation(offset_r, d, base, tmp_pose2);
                    QPointF p(tmp_pose1[dir[0]],tmp_pose1[dir[1]]);
                    QPointF p1(d[dir[0]],d[dir[1]]);
                    QPointF p2(tmp_pose2[dir[0]],tmp_pose2[dir[1]]);
                    if(!InsidePolygon(polygon, p))
                    {
                        inside = false;
//                        ui->textEdit->append(QString(u8"客户端：%1").arg(5));
                    }
                    else if(!InsidePolygon(polygon, p1))
                    {
                        inside = false;
//                        ui->textEdit->append(QString(u8"客户端：%1").arg(6));
                    }
                    else if(!InsidePolygon(polygon, p2))
                    {
                        inside = false;
//                        ui->textEdit->append(QString(u8"客户端：%1").arg(7));
                    }
                    else
                    {
                        inside = true;
                    }
                    if(inside)
                    {
                        v[i]=1;//int = 1;
                    }
                    else
                    {
                        v[i]=0;
                    }
                }
           }

            for (int i = 0; i < 6; i++)
            {
                tmp_pose[i] = tmp_pose[i] + xyze[i]*v[i];
            }
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                tmp_pose[i] = tmp_pose[i] + xyze[i];
            }
        }

        coordinate_transformation(tmp_pose, WorkPiece_pos, base, dst_pose);
        inverse_ext(dst_joints, dst_pose, joints);
        if(abs(joints[5])>0.26&&joints[3]>0.2&&joints[3]< 2.9&&abs(joints[0])< 3.03&&abs(joints[1])< 1.2&&abs(joints[2])< 3.03&&abs(joints[4])< 3.03&&abs(joints[6])< 3.03)
        {
            memcpy(p_pose, tmp_pose, 48);
            memcpy(dst_joints, joints, 56);
            servoJ_ex(dst_joints, 0.002, 0.06, 650, false);
            QThread::usleep(1500);
        }
        else
        {
            memcpy(tmp_pose, p_pose, 48);
            servoJ_ex(dst_joints, 0.002, 0.06, 650, false);
            QThread::usleep(1500);
        }
        QApplication::processEvents();
    }

    Running = false;
    return 0;

    ui->calibrate_zero->setEnabled(true);

}

int Widget::direct_free_drive(bool run, bool is_virtual_wall, int *direction)
{
    if(!Running && run)
    {
        if(getLinkState())
        {
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("-131");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-131));
                is_rec = false;
            }
            robot_link_state = false;
            return -131;
        }
        else
        {
            robot_link_state = true;
        }
        if(releaseBrake())
        {
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("-132");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-132));
                is_rec = false;
            }
            return -132;
        }
        if(is_virtual_wall)
        {
            Virtual_wall = true;
        }
        else
        {
            Virtual_wall = false;
        }

        isok = false;

        if(!run_six_force_info)
        {
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("-133");
                ui->textEdit->append(QString(u8"客户端：%1").arg(-133));
                is_rec = false;
            }
            return -133;
        }
        isok = true;

        int ret = admittance(direction);
        return ret;

    }
    else
    {
        if(!run)
        {
            isok = false;
            run_six_force(false, false);
            if(localsocket->isValid()&&is_rec)
            {
                localsocket->write("0");
                ui->textEdit->append(QString(u8"客户端：%1").arg(0));
                is_rec = false;
            }
            return 0;
        }
        if(localsocket->isValid()&&is_rec)
        {
            localsocket->write("-130");
            ui->textEdit->append(QString(u8"客户端：%1").arg(-130));
            is_rec = false;
        }
        return -130;
    }
}

int Widget::run_six_force(bool open,  bool compensate)
{
    if(open)
    {
        if(run_six_force_info)
        {
            return 0;
        }
        myThread = new Six_axis_sensor;
        if(compensate)
        {
            myThread->is_compensate = true;//is_compensate为true获取补偿后的数据
        }
        else
        {
            myThread->is_compensate = false;//is_compensate为true获取补偿后的数据
        }

        connect(myThread, &Six_axis_sensor::signal_message, this, &Widget::receive_data, Qt::DirectConnection);
        myThread->start();
        QThread::msleep(300);
        if(myThread->isRunning())
        {
            if(!this->isHidden())
            {
                timer->start(5);
            }
            if(compensate)
            {
                run_six_force_info = true;
            }
            else
            {
                run_six_force_info = false;
            }
            return 0;
        }
        else
        {
            run_six_force_info = false;
            if(f[0] > 98)
            {
                qDebug() << "six_sensor_disconnect";
                return 99;
            }
            else if(f[0] > 97)
            {
                qDebug() << "six_sensor Data remains unchanged";
                return 98;
            }
            else if(f[0] > 96)
            {
                qDebug() << "six_sensor_get_tcppos_fail";
                return 97;
            }
            else if(f[0] > 95)
            {
                qDebug() << "six_sensor_frame conversion_fail";
                return 96;
            }
            else
            {
                return -1;
            }

        }
    }
    else
    {
        if(myThread)
        {
            myThread->isok_sub = false;
            if(myThread->isRunning())
            {
                myThread->requestInterruption();
                myThread->wait();
            }
            myThread = nullptr;
            delete myThread;
            if(timer->isActive())
            {
                timer->stop();
            }
        }
        run_six_force_info = false;
        return 0;
    }
}


void Widget::on_get_tcppos_clicked()
{
    QString path = QCoreApplication::applicationDirPath();
//        qDebug() << "Application directory path:" << path;
        ui->textEdit->append(path + "/reslut.txt");

//        QElapsedTimer timer;
//        timer.start();
//        double a[7][7] ={{2.1133 , 0.00807722 , 1.57748 , 0.0645726 , -0.00914877 , -1.39356 , -1.0012},
//                        {0.00807722 , 1.73155 , -0.231669 , 1.66578 , 0.285834 , -1.09411 , 0.88166},
//                        {1.57748 , -0.231669 , 2.18484 , -0.227913 , -1.27918 , -0.808252 , -1.76265},
//                        {0.0645726 , 1.66578 , -0.227913 , 1.83767 , 0.336846 , -1.17735 , 0.894931},
//                        {-0.00914877 , 0.285834 , -1.27918 , 0.336846 , 1.7864 , -0.247008 , 1.46888},
//                        {-1.39356 , -1.09411 , -0.808252 , -1.17735 , -0.247008 , 1.73561 , -1.03854e-10},
//                        {-1.0012 , 0.88166 , -1.76265 , 0.894931 , 1.46888 , -1.03854e-10 , 2}};
//        double b[7][7];
//        for (int y = 0; y < 1; ++y)
//        {
//            matrix_inverse(a,b);
//        }
//        qDebug() << u8"运行时间:" << timer.elapsed() << u8"毫秒";

}
