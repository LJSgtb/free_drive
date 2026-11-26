#include "six_axis_sensor.h"
#include <QCoreApplication>
#include <QElapsedTimer>


float Byte2Float1(QByteArray byte)
{
    float result = 0;
    int size = byte.size();
    char* data_char = byte.data();
    char* p = (char*)&result;
    for(int index = 0; index < size; index++)
    {
        *(p + index) = *(data_char + size - 1 - index);
    }
    return result;
}

char ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

QByteArray QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
        {
            lstr = hstr;
            hstr = '0';
        }
        else
        {
            lstr = str[i].toLatin1();
        }
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == -1) || (lowhexdata == -1))//if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;

        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
        if(i >= len)
        {
            break;
        }
        i++;

    }
    senddata.resize(hexdatalen);
    return senddata;
}
//求行列式
double det1(int n, double *aa)//n为阶数，aa为矩阵,求行列式
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
        sum += flag* aa[arow*n] * det1(n - 1, bb);//aa第一列各元素与其代数余子式积的和即为行列式
    }
    delete[]bb;
    return sum;
}
//矩阵加法
template<size_t arows, size_t acols, size_t brows, size_t bcols, size_t ret_row, size_t ret_bcol>
void matrix_plus1(double (&a)[arows][acols], double (&b)[brows][bcols],  double (&ret)[ret_row][ret_bcol])
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
void matrix_reduce1(double (&a)[arows][acols], double (&b)[brows][bcols],  double (&ret)[ret_row][ret_bcol])
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
void matrix_multiplication1(double (&a)[arows][acols], double (&b)[brows][bcols],  double (&ret)[ret_row][ret_bcol])
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
void matrix_transpose1(double (&a)[arows][acols],  double (&ret)[ret_row][ret_bcol])
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
void matrix_inverse1(double (&a)[arows][acols],  double (&ret)[ret_row][ret_bcol])
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
//    double temp2[(acols-1)*(arows-1)];
    double* temp2 = new double[(acols-1)*(arows-1)];
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
            ret[i][j] = pow(-1,i+j)*det1(acols-1,temp2);
        }
    }
    //矩阵转置
    matrix_transpose1(ret, ret1);
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
    double determinant = det1(acols,temp);
    for(m = 0; m < arows; m++)
    {
        for(n = 0; n < acols; n++)
        {
            ret[m][n] = ret[m][n]/determinant;
        }
    }
    delete[] temp2;

//    qDebug() << res;
//    qDebug() << "__________________________________________";

}

Six_axis_sensor::Six_axis_sensor(QObject *parent)
    : QThread{parent}
{

}

Six_axis_sensor::~Six_axis_sensor()
{

}

void Six_axis_sensor::run()
{
    if(is_compensate)
    {
        int ret = getLinkState();
        if(ret == 0)
        {
            qDebug("network connected \n");
        }
        else
        {
            qDebug("network disconnected \n");
            return;
        }

        //---------------------------读取六维力标定结果----------------------------------------------------------------------
        QString path = QCoreApplication::applicationDirPath() + "/reslut.txt";
        QFile file(path);
        QVector<QVector<double>> ff;
        QVector<double> ft;

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
                    ft.push_back(item.toDouble());
                }
                ff.push_back(ft);
                ft.clear();
            }
            file.close();
        }

        if(ff.length() == 6)
        {
            Center_of_gravity_sub[0] = ff[0][0]; Center_of_gravity_sub[1] = ff[0][1]; Center_of_gravity_sub[2] = ff[0][2];
            K1K2K3_sub[0] = ff[1][0]; K1K2K3_sub[1] = ff[1][1]; K1K2K3_sub[2] = ff[1][2];
            zero_offset_sub[0] = ff[2][0]; zero_offset_sub[1] = ff[2][1]; zero_offset_sub[2] = ff[2][2];
            zero_offset_sub[3] = ff[2][3]; zero_offset_sub[4] = ff[2][4]; zero_offset_sub[5] = ff[2][5];
            LxLyLz_sub[0] = ff[3][0]; LxLyLz_sub[1] = ff[3][1]; LxLyLz_sub[2] = ff[3][2];
            G_sub = ff[4][0];
            UV_sub[0] = ff[5][0]; UV_sub[1] = ff[5][1];
            qDebug() << "数据读取成功";
        }
        else
        {
            return;
        }
    }

//---------------------------卡尔曼滤波准备数据----------------------------------------------------------------------
    double tmp1_22[2][2], tmp2_22[2][2], tmp3_11[1][1], tmp4_21[2][1], tmp5_12[1][2], tmp6_21[2][1], identity_22[2][2];

    identity_22[0][0] = 1;identity_22[0][1] = 0;
    identity_22[0][1] = 0;identity_22[1][1] = 1;

    double x_tmp_11[1][1], S_11[1][1], K_21[2][1];
    vector<double[2][1]> hat_pred_21(6);
    vector<double[2][2]> P_pred_22(6);


    //     定义系统动力学模型
    double A_22[2][2];
    A_22[0][0] = 1; A_22[0][1] = 1;
    A_22[1][0] = 0; A_22[1][1] = 1;
    double A_transpose_22[2][2];
    A_transpose_22[0][0] = 1; A_transpose_22[0][1] = 0;
    A_transpose_22[1][0] = 1; A_transpose_22[1][1] = 1;
    double H_12[1][2];
    H_12[0][0] = 1; H_12[0][1] = 0;
    double H_transpose_21[2][1];
    H_transpose_21[0][0] = 1; H_transpose_21[1][0] = 0;
    // 定义系统噪声和观测噪声的协方差矩阵
    double Q_22[2][2];
    Q_22[0][0] = 0.001; Q_22[0][1] = 0;
    Q_22[1][0] = 0; Q_22[1][1] = 0.001;
    double R_11[1][1];
    R_11[0][0] = 1000000;//越大滤波效果越好


    // 初始化状态估计和协方差矩阵
    vector<double[2][1]> hat_21(6);
    vector<double[2][2]> P_22(6);
    P_22[0][0][0] = 1; P_22[0][0][1] = 0; P_22[0][1][0] = 0; P_22[0][1][1] = 1;
    P_22[1][0][0] = 1; P_22[1][0][1] = 0; P_22[1][1][0] = 0; P_22[1][1][1] = 1;
    P_22[2][0][0] = 1; P_22[2][0][1] = 0; P_22[2][1][0] = 0; P_22[2][1][1] = 1;
    P_22[3][0][0] = 1; P_22[3][0][1] = 0; P_22[3][1][0] = 0; P_22[3][1][1] = 1;
    P_22[4][0][0] = 1; P_22[4][0][1] = 0; P_22[4][1][0] = 0; P_22[4][1][1] = 1;
    P_22[5][0][0] = 1; P_22[5][0][1] = 0; P_22[5][1][0] = 0; P_22[5][1][1] = 1;

//---------------------------接收六维力数据----------------------------------------------------------------------

    QByteArray tmp, x_F, y_F, z_F, x_M, y_M, z_M;
    QNetworkDatagram datagram;

    double matrix_E[3][3] = {{0.0}};
    double L[3][1] = {{0.0}};
    double GCxyz[3][1] = {{0.0}};

    L[0][0] = LxLyLz_sub[0];
    L[1][0] = LxLyLz_sub[1];
    L[2][0] = LxLyLz_sub[2];


    double pos[6] = {0.0};
    double matrix[16]={0.0};
    double force[6] = {0.0};
    double force_ext[6] = {0.0};
    double f[6] = {0.0};
    int monitor_six = 0, monitor_robot = 0, monitor_six_fd = 0;
    double disconnect[6] = {99,99,99,99,99,99};
    double fangdai_send[6] = {98,98,98,98,98,98};
    double get_tcp_fail[6] = {97,97,97,97,97,97};
    double conversion_fail[6] = {96,96,96,96,96,96};
    double fangdai = 0;

    QUdpSocket mSocket_sub;
    if(mSocket_sub.bind(QHostAddress::AnyIPv4,8886))
    {
        qDebug() << "bind_success";
    }
    mSocket_sub.writeDatagram(QString2Hex("48 AA 0D 0A"),QHostAddress("192.168.1.101"),5152);

    int ret = 0;
    double t = 0;
    isok_sub = true;
    while(isok_sub)
    {
        if(mSocket_sub.hasPendingDatagrams())
        {
            datagram.clear();
            datagram = mSocket_sub.receiveDatagram(-1);
            tmp = datagram.data();

            x_F = tmp.mid(2, 4);
            std::reverse(x_F.begin(), x_F.end());
            y_F = tmp.mid(6, 4);
            std::reverse(y_F.begin(), y_F.end());
            z_F = tmp.mid(10, 4);
            std::reverse(z_F.begin(), z_F.end());
            x_M = tmp.mid(14, 4);
            std::reverse(x_M.begin(), x_M.end());
            y_M = tmp.mid(18, 4);
            std::reverse(y_M.begin(), y_M.end());
            z_M = tmp.mid(22, 4);
            std::reverse(z_M.begin(), z_M.end());

            force[0] = Byte2Float1(x_F) * 9.8;
            force[1] = Byte2Float1(y_F) * 9.8;
            force[2] = Byte2Float1(z_F) * 9.8;
            force[3] = Byte2Float1(x_M) * 9.8;
            force[4] = Byte2Float1(y_M) * 9.8;
            force[5] = Byte2Float1(z_M) * 9.8;

            monitor_six = 0;
            //fangdai
//            qDebug() << force[0];
            if(abs(force[0] - fangdai) > 0.01)
            {
                fangdai = force[0];
                monitor_six_fd = 0;
            }
            else
            {
                monitor_six_fd++;
                if(monitor_six_fd > 10)
                {
                    signal_message(fangdai_send);
                    qDebug() << "six_force daizhu";
                    return;
                }
            }

        }
        else
        {
            //监测六维力是否连接
            monitor_six++;
            if(monitor_six > 20)
            {
                qDebug() << "six_force disconnected";
                signal_message(disconnect);
                return;
            }
            QThread::usleep(120);
        }

        if(abs(force[0]-t) < 0.0001)
        {
            continue;
        }
        t = force[0];

//-----------------------------计算重力补偿后的外力------------------------------------------------------------
        if(is_compensate)
        {

            ret = getTcpPos(pos);
            if(ret != 0)
            {
                monitor_robot+=1;
                if(monitor_robot > 3)
                {
                    qDebug() << "get tcp fail";
                    signal_message(get_tcp_fail);
                    return;
                }
            }
            else
            {
                monitor_robot = 0;
            }

            ret = pose2Homogeneous(pos, matrix);
            if(ret != 0)
            {
                qDebug() << "conversion fail";
                signal_message(conversion_fail);
                return;
            }

            matrix_E[0][0] = matrix[0];matrix_E[0][1] = matrix[1];matrix_E[0][2] = matrix[2];
            matrix_E[1][0] = matrix[4];matrix_E[1][1] = matrix[5];matrix_E[1][2] = matrix[6];
            matrix_E[2][0] = matrix[8];matrix_E[2][1] = matrix[9];matrix_E[2][2] = matrix[10];

            //负载重力在传感器坐标系轴上的力分量
            matrix_multiplication1(matrix_E, L, GCxyz);

            //负载重力在传感器坐标系轴上的力矩分量
            double GMx = (GCxyz[2][0] * Center_of_gravity_sub[1]) - (GCxyz[1][0] * Center_of_gravity_sub[2]);
            double GMy = (GCxyz[0][0] * Center_of_gravity_sub[2]) - (GCxyz[2][0] * Center_of_gravity_sub[0]);
            double GMz = (GCxyz[1][0] * Center_of_gravity_sub[0]) - (GCxyz[0][0] * Center_of_gravity_sub[1]);

            force_ext[0] = force[0] - zero_offset_sub[0] - GCxyz[0][0];
            force_ext[1] = force[1] - zero_offset_sub[1] - GCxyz[1][0];
            force_ext[2] = force[2] - zero_offset_sub[2] - GCxyz[2][0];
            force_ext[3] = force[3] - zero_offset_sub[3] - GMx;
            force_ext[4] = force[4] - zero_offset_sub[4] - GMy;
            force_ext[5] = force[5] - zero_offset_sub[5] - GMz;


//-----------------------------卡尔曼滤波------------------------------------------------------------
            int adjust[6]={1,1,1,1,1,1};
            for (int i = 0; i < 6; i++)
            {
                // 预测步骤
                matrix_multiplication1(A_22, hat_21[i], hat_pred_21[i]);
                matrix_multiplication1(A_22, P_22[i], tmp1_22);
                matrix_multiplication1(tmp1_22, A_transpose_22, tmp2_22);
                matrix_plus1(tmp2_22, Q_22, P_pred_22[i]);


                // 更新步骤
                matrix_multiplication1(H_12, hat_pred_21[i], tmp3_11);
                x_tmp_11[0][0] = force_ext[i] - tmp3_11[0][0];

                matrix_multiplication1(H_12, P_pred_22[i],tmp5_12);
                matrix_multiplication1(tmp5_12, H_transpose_21,tmp3_11);
                S_11[0][0] = tmp3_11[0][0] +  R_11[0][0]*adjust[i];

                matrix_multiplication1(P_pred_22[i], H_transpose_21,tmp4_21);
                matrix_inverse1(S_11,tmp3_11);
                matrix_multiplication1(tmp4_21, tmp3_11,K_21);

                matrix_multiplication1(K_21, x_tmp_11,tmp4_21);
                matrix_plus1(hat_pred_21[i], tmp4_21,hat_21[i]);
                matrix_multiplication1(K_21, H_12,tmp1_22);
                matrix_reduce1(identity_22, tmp1_22,tmp2_22);
                matrix_multiplication1(tmp2_22, P_pred_22[i],P_22[i]);
            }
    //---------------------------------------------------------------------

            f[0] = round(hat_21[0][0][0]*10)/10;
            f[1] = round(hat_21[1][0][0]*10)/10;
            f[2] = round(hat_21[2][0][0]*10)/10;
            f[3] = round(hat_21[3][0][0]*10)/10;
            f[4] = round(hat_21[4][0][0]*10)/10;
            f[5] = round(hat_21[5][0][0]*10)/10;


            signal_message(f);
        }
        else
        {
            signal_message(force);
        }

//            QThread::usleep(100);
//            QApplication::processEvents();
    }

}

void Six_axis_sensor::stop()
{
    isok_sub = false;
}
