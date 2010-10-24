/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun Oct 24 11:35:05 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label_UpdateDelay;
    QSpinBox *spinBox_UpdateDelay;
    QLabel *label_ms;
    QSpinBox *spinBox_ReconnectDelay;
    QLabel *label_sec;
    QLabel *label_height;
    QSpinBox *spinBox_HeightAmbilight;
    QLabel *label_sec_4;
    QLabel *label_width;
    QSpinBox *spinBox_WidthAmbilight;
    QLabel *label_sec_5;
    QCheckBox *checkBox_ShowPixelsAmbilight;
    QLabel *label_UpdateDelayEval;
    QLineEdit *lineEdit_RefreshAmbilihtEvaluated;
    QLabel *label_hz;
    QLabel *label_Reconnect;
    QLabel *label_USBSendDataTimeout;
    QLabel *label_sec_6;
    QDoubleSpinBox *doubleSpinBoxUsbSendDataTimeout;
    QSlider *horizontalSliderWidth;
    QSlider *horizontalSliderHeight;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_Close;
    QSpacerItem *horizontalSpacer;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(341, 375);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_UpdateDelay = new QLabel(centralwidget);
        label_UpdateDelay->setObjectName(QString::fromUtf8("label_UpdateDelay"));

        gridLayout->addWidget(label_UpdateDelay, 0, 0, 1, 1);

        spinBox_UpdateDelay = new QSpinBox(centralwidget);
        spinBox_UpdateDelay->setObjectName(QString::fromUtf8("spinBox_UpdateDelay"));
        spinBox_UpdateDelay->setMinimum(0);
        spinBox_UpdateDelay->setMaximum(1000);
        spinBox_UpdateDelay->setValue(20);

        gridLayout->addWidget(spinBox_UpdateDelay, 0, 1, 1, 1);

        label_ms = new QLabel(centralwidget);
        label_ms->setObjectName(QString::fromUtf8("label_ms"));

        gridLayout->addWidget(label_ms, 0, 2, 1, 1);

        spinBox_ReconnectDelay = new QSpinBox(centralwidget);
        spinBox_ReconnectDelay->setObjectName(QString::fromUtf8("spinBox_ReconnectDelay"));
        spinBox_ReconnectDelay->setMinimum(1);
        spinBox_ReconnectDelay->setValue(2);

        gridLayout->addWidget(spinBox_ReconnectDelay, 2, 1, 1, 1);

        label_sec = new QLabel(centralwidget);
        label_sec->setObjectName(QString::fromUtf8("label_sec"));

        gridLayout->addWidget(label_sec, 2, 2, 1, 1);

        label_height = new QLabel(centralwidget);
        label_height->setObjectName(QString::fromUtf8("label_height"));

        gridLayout->addWidget(label_height, 6, 0, 1, 1);

        spinBox_HeightAmbilight = new QSpinBox(centralwidget);
        spinBox_HeightAmbilight->setObjectName(QString::fromUtf8("spinBox_HeightAmbilight"));
        spinBox_HeightAmbilight->setMinimum(1);
        spinBox_HeightAmbilight->setMaximum(800);
        spinBox_HeightAmbilight->setValue(400);

        gridLayout->addWidget(spinBox_HeightAmbilight, 6, 1, 1, 1);

        label_sec_4 = new QLabel(centralwidget);
        label_sec_4->setObjectName(QString::fromUtf8("label_sec_4"));

        gridLayout->addWidget(label_sec_4, 6, 2, 1, 1);

        label_width = new QLabel(centralwidget);
        label_width->setObjectName(QString::fromUtf8("label_width"));

        gridLayout->addWidget(label_width, 8, 0, 1, 1);

        spinBox_WidthAmbilight = new QSpinBox(centralwidget);
        spinBox_WidthAmbilight->setObjectName(QString::fromUtf8("spinBox_WidthAmbilight"));
        spinBox_WidthAmbilight->setMinimum(1);
        spinBox_WidthAmbilight->setMaximum(1280);
        spinBox_WidthAmbilight->setValue(200);

        gridLayout->addWidget(spinBox_WidthAmbilight, 8, 1, 1, 1);

        label_sec_5 = new QLabel(centralwidget);
        label_sec_5->setObjectName(QString::fromUtf8("label_sec_5"));

        gridLayout->addWidget(label_sec_5, 8, 2, 1, 1);

        checkBox_ShowPixelsAmbilight = new QCheckBox(centralwidget);
        checkBox_ShowPixelsAmbilight->setObjectName(QString::fromUtf8("checkBox_ShowPixelsAmbilight"));

        gridLayout->addWidget(checkBox_ShowPixelsAmbilight, 5, 0, 1, 3);

        label_UpdateDelayEval = new QLabel(centralwidget);
        label_UpdateDelayEval->setObjectName(QString::fromUtf8("label_UpdateDelayEval"));

        gridLayout->addWidget(label_UpdateDelayEval, 1, 0, 1, 1);

        lineEdit_RefreshAmbilihtEvaluated = new QLineEdit(centralwidget);
        lineEdit_RefreshAmbilihtEvaluated->setObjectName(QString::fromUtf8("lineEdit_RefreshAmbilihtEvaluated"));
        lineEdit_RefreshAmbilihtEvaluated->setEnabled(true);
        lineEdit_RefreshAmbilihtEvaluated->setReadOnly(true);

        gridLayout->addWidget(lineEdit_RefreshAmbilihtEvaluated, 1, 1, 1, 1);

        label_hz = new QLabel(centralwidget);
        label_hz->setObjectName(QString::fromUtf8("label_hz"));

        gridLayout->addWidget(label_hz, 1, 2, 1, 1);

        label_Reconnect = new QLabel(centralwidget);
        label_Reconnect->setObjectName(QString::fromUtf8("label_Reconnect"));

        gridLayout->addWidget(label_Reconnect, 2, 0, 1, 1);

        label_USBSendDataTimeout = new QLabel(centralwidget);
        label_USBSendDataTimeout->setObjectName(QString::fromUtf8("label_USBSendDataTimeout"));

        gridLayout->addWidget(label_USBSendDataTimeout, 3, 0, 1, 1);

        label_sec_6 = new QLabel(centralwidget);
        label_sec_6->setObjectName(QString::fromUtf8("label_sec_6"));

        gridLayout->addWidget(label_sec_6, 3, 2, 1, 1);

        doubleSpinBoxUsbSendDataTimeout = new QDoubleSpinBox(centralwidget);
        doubleSpinBoxUsbSendDataTimeout->setObjectName(QString::fromUtf8("doubleSpinBoxUsbSendDataTimeout"));
        doubleSpinBoxUsbSendDataTimeout->setMinimum(0.2);
        doubleSpinBoxUsbSendDataTimeout->setMaximum(20);
        doubleSpinBoxUsbSendDataTimeout->setSingleStep(0.2);
        doubleSpinBoxUsbSendDataTimeout->setValue(1.2);

        gridLayout->addWidget(doubleSpinBoxUsbSendDataTimeout, 3, 1, 1, 1);

        horizontalSliderWidth = new QSlider(centralwidget);
        horizontalSliderWidth->setObjectName(QString::fromUtf8("horizontalSliderWidth"));
        horizontalSliderWidth->setMinimum(1);
        horizontalSliderWidth->setMaximum(1280);
        horizontalSliderWidth->setValue(200);
        horizontalSliderWidth->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSliderWidth, 9, 0, 1, 3);

        horizontalSliderHeight = new QSlider(centralwidget);
        horizontalSliderHeight->setObjectName(QString::fromUtf8("horizontalSliderHeight"));
        horizontalSliderHeight->setMinimum(1);
        horizontalSliderHeight->setMaximum(800);
        horizontalSliderHeight->setValue(400);
        horizontalSliderHeight->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSliderHeight, 7, 0, 1, 3);


        verticalLayout->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(37, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pushButton_Close = new QPushButton(centralwidget);
        pushButton_Close->setObjectName(QString::fromUtf8("pushButton_Close"));

        horizontalLayout->addWidget(pushButton_Close);

        horizontalSpacer = new QSpacerItem(37, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralwidget);
        QWidget::setTabOrder(spinBox_UpdateDelay, spinBox_ReconnectDelay);
        QWidget::setTabOrder(spinBox_ReconnectDelay, checkBox_ShowPixelsAmbilight);
        QWidget::setTabOrder(checkBox_ShowPixelsAmbilight, spinBox_HeightAmbilight);
        QWidget::setTabOrder(spinBox_HeightAmbilight, spinBox_WidthAmbilight);
        QWidget::setTabOrder(spinBox_WidthAmbilight, pushButton_Close);

        retranslateUi(MainWindow);
        QObject::connect(horizontalSliderWidth, SIGNAL(valueChanged(int)), spinBox_WidthAmbilight, SLOT(setValue(int)));
        QObject::connect(horizontalSliderHeight, SIGNAL(valueChanged(int)), spinBox_HeightAmbilight, SLOT(setValue(int)));
        QObject::connect(spinBox_WidthAmbilight, SIGNAL(valueChanged(int)), horizontalSliderWidth, SLOT(setValue(int)));
        QObject::connect(spinBox_HeightAmbilight, SIGNAL(valueChanged(int)), horizontalSliderHeight, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Ambilight USB - Settings", 0, QApplication::UnicodeUTF8));
        label_UpdateDelay->setText(QApplication::translate("MainWindow", "Refresh ambilight delay", 0, QApplication::UnicodeUTF8));
        label_ms->setText(QApplication::translate("MainWindow", "ms", 0, QApplication::UnicodeUTF8));
        label_sec->setText(QApplication::translate("MainWindow", "secs", 0, QApplication::UnicodeUTF8));
        label_height->setText(QApplication::translate("MainWindow", "Height", 0, QApplication::UnicodeUTF8));
        label_sec_4->setText(QApplication::translate("MainWindow", "pixels", 0, QApplication::UnicodeUTF8));
        label_width->setText(QApplication::translate("MainWindow", "Width", 0, QApplication::UnicodeUTF8));
        label_sec_5->setText(QApplication::translate("MainWindow", "pixels", 0, QApplication::UnicodeUTF8));
        checkBox_ShowPixelsAmbilight->setText(QApplication::translate("MainWindow", "Show pixels for ambilight", 0, QApplication::UnicodeUTF8));
        label_UpdateDelayEval->setText(QApplication::translate("MainWindow", "Refresh ambilight evaluated", 0, QApplication::UnicodeUTF8));
        label_hz->setText(QApplication::translate("MainWindow", "Hz", 0, QApplication::UnicodeUTF8));
        label_Reconnect->setText(QApplication::translate("MainWindow", "Reconnect USB delay", 0, QApplication::UnicodeUTF8));
        label_USBSendDataTimeout->setText(QApplication::translate("MainWindow", "USB send data timeout", 0, QApplication::UnicodeUTF8));
        label_sec_6->setText(QApplication::translate("MainWindow", "secs", 0, QApplication::UnicodeUTF8));
        pushButton_Close->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
