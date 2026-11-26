/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *six_force_calibration;
    QPushButton *pushButton_2;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTextEdit *textEdit;
    QPushButton *get_tcppos;
    QPushButton *External_force_value;
    QLineEdit *lineEdit;
    QLCDNumber *lcdNumber;
    QLCDNumber *lcdNumber_2;
    QLCDNumber *lcdNumber_3;
    QLCDNumber *lcdNumber_4;
    QLCDNumber *lcdNumber_5;
    QLCDNumber *lcdNumber_6;
    QPushButton *enter_admittance;
    QPushButton *run_six_force;
    QRadioButton *radioButton;
    QPushButton *calibrate_zero;
    QLabel *zero_x;
    QLabel *zero_y;
    QLabel *zero_z;
    QLabel *zero_rx;
    QLabel *zero_ry;
    QLabel *zero_rz;
    QRadioButton *X;
    QRadioButton *Y;
    QRadioButton *Z;
    QRadioButton *RZ;
    QRadioButton *RX;
    QRadioButton *RY;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        six_force_calibration = new QPushButton(Widget);
        six_force_calibration->setObjectName(QString::fromUtf8("six_force_calibration"));
        six_force_calibration->setGeometry(QRect(600, 320, 101, 41));
        pushButton_2 = new QPushButton(Widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(600, 270, 101, 41));
        layoutWidget = new QWidget(Widget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 571, 511));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        textEdit = new QTextEdit(layoutWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout->addWidget(textEdit);

        get_tcppos = new QPushButton(Widget);
        get_tcppos->setObjectName(QString::fromUtf8("get_tcppos"));
        get_tcppos->setGeometry(QRect(710, 540, 81, 41));
        External_force_value = new QPushButton(Widget);
        External_force_value->setObjectName(QString::fromUtf8("External_force_value"));
        External_force_value->setGeometry(QRect(0, 540, 91, 41));
        lineEdit = new QLineEdit(Widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(100, 540, 601, 41));
        lcdNumber = new QLCDNumber(Widget);
        lcdNumber->setObjectName(QString::fromUtf8("lcdNumber"));
        lcdNumber->setGeometry(QRect(600, 20, 141, 41));
        lcdNumber->setDigitCount(8);
        lcdNumber_2 = new QLCDNumber(Widget);
        lcdNumber_2->setObjectName(QString::fromUtf8("lcdNumber_2"));
        lcdNumber_2->setGeometry(QRect(600, 60, 141, 41));
        lcdNumber_2->setDigitCount(8);
        lcdNumber_3 = new QLCDNumber(Widget);
        lcdNumber_3->setObjectName(QString::fromUtf8("lcdNumber_3"));
        lcdNumber_3->setGeometry(QRect(600, 100, 141, 41));
        lcdNumber_3->setDigitCount(8);
        lcdNumber_4 = new QLCDNumber(Widget);
        lcdNumber_4->setObjectName(QString::fromUtf8("lcdNumber_4"));
        lcdNumber_4->setGeometry(QRect(600, 140, 141, 41));
        lcdNumber_4->setDigitCount(8);
        lcdNumber_5 = new QLCDNumber(Widget);
        lcdNumber_5->setObjectName(QString::fromUtf8("lcdNumber_5"));
        lcdNumber_5->setGeometry(QRect(600, 180, 141, 41));
        lcdNumber_5->setDigitCount(8);
        lcdNumber_6 = new QLCDNumber(Widget);
        lcdNumber_6->setObjectName(QString::fromUtf8("lcdNumber_6"));
        lcdNumber_6->setGeometry(QRect(600, 220, 141, 41));
        lcdNumber_6->setDigitCount(8);
        enter_admittance = new QPushButton(Widget);
        enter_admittance->setObjectName(QString::fromUtf8("enter_admittance"));
        enter_admittance->setGeometry(QRect(600, 420, 101, 41));
        run_six_force = new QPushButton(Widget);
        run_six_force->setObjectName(QString::fromUtf8("run_six_force"));
        run_six_force->setGeometry(QRect(600, 370, 101, 41));
        radioButton = new QRadioButton(Widget);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setGeometry(QRect(710, 390, 71, 23));
        radioButton->setCheckable(true);
        radioButton->setChecked(true);
        radioButton->setAutoExclusive(false);
        calibrate_zero = new QPushButton(Widget);
        calibrate_zero->setObjectName(QString::fromUtf8("calibrate_zero"));
        calibrate_zero->setEnabled(false);
        calibrate_zero->setGeometry(QRect(710, 420, 71, 41));
        zero_x = new QLabel(Widget);
        zero_x->setObjectName(QString::fromUtf8("zero_x"));
        zero_x->setGeometry(QRect(750, 20, 41, 41));
        zero_y = new QLabel(Widget);
        zero_y->setObjectName(QString::fromUtf8("zero_y"));
        zero_y->setGeometry(QRect(750, 60, 41, 41));
        zero_z = new QLabel(Widget);
        zero_z->setObjectName(QString::fromUtf8("zero_z"));
        zero_z->setGeometry(QRect(750, 100, 41, 41));
        zero_rx = new QLabel(Widget);
        zero_rx->setObjectName(QString::fromUtf8("zero_rx"));
        zero_rx->setGeometry(QRect(750, 140, 41, 41));
        zero_ry = new QLabel(Widget);
        zero_ry->setObjectName(QString::fromUtf8("zero_ry"));
        zero_ry->setGeometry(QRect(750, 180, 41, 41));
        zero_rz = new QLabel(Widget);
        zero_rz->setObjectName(QString::fromUtf8("zero_rz"));
        zero_rz->setGeometry(QRect(750, 220, 41, 41));
        X = new QRadioButton(Widget);
        X->setObjectName(QString::fromUtf8("X"));
        X->setGeometry(QRect(600, 470, 31, 23));
        X->setCheckable(true);
        X->setChecked(true);
        X->setAutoExclusive(false);
        Y = new QRadioButton(Widget);
        Y->setObjectName(QString::fromUtf8("Y"));
        Y->setGeometry(QRect(660, 470, 31, 23));
        Y->setCheckable(true);
        Y->setChecked(false);
        Y->setAutoExclusive(false);
        Z = new QRadioButton(Widget);
        Z->setObjectName(QString::fromUtf8("Z"));
        Z->setGeometry(QRect(710, 470, 31, 23));
        Z->setCheckable(true);
        Z->setChecked(true);
        Z->setAutoExclusive(false);
        RZ = new QRadioButton(Widget);
        RZ->setObjectName(QString::fromUtf8("RZ"));
        RZ->setGeometry(QRect(710, 500, 41, 23));
        RZ->setCheckable(true);
        RZ->setChecked(false);
        RZ->setAutoExclusive(false);
        RX = new QRadioButton(Widget);
        RX->setObjectName(QString::fromUtf8("RX"));
        RX->setGeometry(QRect(600, 500, 41, 23));
        RX->setCheckable(true);
        RX->setChecked(false);
        RX->setAutoExclusive(false);
        RY = new QRadioButton(Widget);
        RY->setObjectName(QString::fromUtf8("RY"));
        RY->setGeometry(QRect(660, 500, 41, 23));
        RY->setCheckable(true);
        RY->setChecked(false);
        RY->setAutoExclusive(false);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        six_force_calibration->setText(QCoreApplication::translate("Widget", "\345\205\255\347\273\264\345\212\233\346\240\207\345\256\232", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245\346\234\272\345\231\250\344\272\272", nullptr));
        label->setText(QCoreApplication::translate("Widget", "\346\266\210\346\201\257\346\241\206", nullptr));
        get_tcppos->setText(QCoreApplication::translate("Widget", "\350\216\267\345\217\226TCP\344\275\215\347\275\256", nullptr));
        External_force_value->setText(QCoreApplication::translate("Widget", "\345\244\226\345\212\233\346\204\237\347\237\245", nullptr));
        enter_admittance->setText(QCoreApplication::translate("Widget", "\350\277\233\345\205\245\345\256\232\345\220\221\345\257\274\347\272\263", nullptr));
        run_six_force->setText(QCoreApplication::translate("Widget", "\345\274\200\345\220\257\345\205\255\347\273\264\345\212\233", nullptr));
        radioButton->setText(QCoreApplication::translate("Widget", "\351\207\215\345\212\233\350\241\245\345\201\277", nullptr));
        calibrate_zero->setText(QCoreApplication::translate("Widget", "\346\240\241\345\207\206\351\233\266\345\201\217", nullptr));
        zero_x->setText(QCoreApplication::translate("Widget", "0", nullptr));
        zero_y->setText(QCoreApplication::translate("Widget", "0", nullptr));
        zero_z->setText(QCoreApplication::translate("Widget", "0", nullptr));
        zero_rx->setText(QCoreApplication::translate("Widget", "0", nullptr));
        zero_ry->setText(QCoreApplication::translate("Widget", "0", nullptr));
        zero_rz->setText(QCoreApplication::translate("Widget", "0", nullptr));
        X->setText(QCoreApplication::translate("Widget", "X", nullptr));
        Y->setText(QCoreApplication::translate("Widget", "Y", nullptr));
        Z->setText(QCoreApplication::translate("Widget", "Z", nullptr));
        RZ->setText(QCoreApplication::translate("Widget", "RZ", nullptr));
        RX->setText(QCoreApplication::translate("Widget", "RX", nullptr));
        RY->setText(QCoreApplication::translate("Widget", "RY", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
