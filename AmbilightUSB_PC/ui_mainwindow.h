/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed Oct 27 15:50:19 2010
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
#include <QtGui/QComboBox>
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
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabSoftwareOptions;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *label_UpdateDelay;
    QSpinBox *spinBox_UpdateDelay;
    QLabel *label_ms;
    QLabel *label_height;
    QSpinBox *spinBox_HeightAmbilight;
    QLabel *label_sec_4;
    QLabel *label_width;
    QLabel *label_sec_5;
    QLabel *label_UpdateDelayEval;
    QLineEdit *lineEdit_RefreshAmbilihtEvaluated;
    QLabel *label_hz;
    QSlider *horizontalSliderHeight;
    QCheckBox *checkBox_ShowPixelsAmbilight;
    QSpinBox *spinBox_WidthAmbilight;
    QSlider *horizontalSliderWidth;
    QGridLayout *gridLayout_3;
    QLabel *label_WB_Red;
    QLabel *label_WB_Green;
    QLabel *label_WB_Blue;
    QDoubleSpinBox *doubleSpinBox_WB_Red;
    QDoubleSpinBox *doubleSpinBox_WB_Green;
    QDoubleSpinBox *doubleSpinBox_WB_Blue;
    QLabel *label_WhiteBalance;
    QSpacerItem *verticalSpacer;
    QWidget *tabHardwareOptions;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout_2;
    QLabel *label_HW_TimPrescaller;
    QComboBox *comboBox_HW_Prescaller;
    QSlider *horizontalSlider_HW_Prescaller;
    QLabel *label_HW_TimerOCR;
    QSpinBox *spinBox_HW_OCR;
    QLabel *label_HW_Ticks;
    QLabel *label_Reconnect;
    QSpinBox *spinBox_ReconnectDelay;
    QLabel *label_sec;
    QLabel *label_USBSendDataTimeout;
    QDoubleSpinBox *doubleSpinBoxUsbSendDataTimeout;
    QLabel *label_sec_6;
    QSlider *horizontalSlider_HW_OCR;
    QLabel *label_4;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_HW_ColorDepth;
    QSlider *horizontalSlider_HW_ColorDepth;
    QSpinBox *spinBox_HW_ColorDepth;
    QLabel *label_HW_ColorDepth_counts;
    QLineEdit *lineEdit_PWM_Frequency;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_Close;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(360, 391);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabSoftwareOptions = new QWidget();
        tabSoftwareOptions->setObjectName(QString::fromUtf8("tabSoftwareOptions"));
        verticalLayout_2 = new QVBoxLayout(tabSoftwareOptions);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_UpdateDelay = new QLabel(tabSoftwareOptions);
        label_UpdateDelay->setObjectName(QString::fromUtf8("label_UpdateDelay"));

        gridLayout->addWidget(label_UpdateDelay, 0, 0, 1, 1);

        spinBox_UpdateDelay = new QSpinBox(tabSoftwareOptions);
        spinBox_UpdateDelay->setObjectName(QString::fromUtf8("spinBox_UpdateDelay"));
        spinBox_UpdateDelay->setMinimum(0);
        spinBox_UpdateDelay->setMaximum(1000);
        spinBox_UpdateDelay->setValue(20);

        gridLayout->addWidget(spinBox_UpdateDelay, 0, 1, 1, 1);

        label_ms = new QLabel(tabSoftwareOptions);
        label_ms->setObjectName(QString::fromUtf8("label_ms"));

        gridLayout->addWidget(label_ms, 0, 2, 1, 1);

        label_height = new QLabel(tabSoftwareOptions);
        label_height->setObjectName(QString::fromUtf8("label_height"));

        gridLayout->addWidget(label_height, 4, 0, 1, 1);

        spinBox_HeightAmbilight = new QSpinBox(tabSoftwareOptions);
        spinBox_HeightAmbilight->setObjectName(QString::fromUtf8("spinBox_HeightAmbilight"));
        spinBox_HeightAmbilight->setMinimum(1);
        spinBox_HeightAmbilight->setMaximum(800);
        spinBox_HeightAmbilight->setValue(400);

        gridLayout->addWidget(spinBox_HeightAmbilight, 4, 1, 1, 1);

        label_sec_4 = new QLabel(tabSoftwareOptions);
        label_sec_4->setObjectName(QString::fromUtf8("label_sec_4"));

        gridLayout->addWidget(label_sec_4, 4, 2, 1, 1);

        label_width = new QLabel(tabSoftwareOptions);
        label_width->setObjectName(QString::fromUtf8("label_width"));

        gridLayout->addWidget(label_width, 6, 0, 1, 1);

        label_sec_5 = new QLabel(tabSoftwareOptions);
        label_sec_5->setObjectName(QString::fromUtf8("label_sec_5"));

        gridLayout->addWidget(label_sec_5, 6, 2, 1, 1);

        label_UpdateDelayEval = new QLabel(tabSoftwareOptions);
        label_UpdateDelayEval->setObjectName(QString::fromUtf8("label_UpdateDelayEval"));

        gridLayout->addWidget(label_UpdateDelayEval, 1, 0, 1, 1);

        lineEdit_RefreshAmbilihtEvaluated = new QLineEdit(tabSoftwareOptions);
        lineEdit_RefreshAmbilihtEvaluated->setObjectName(QString::fromUtf8("lineEdit_RefreshAmbilihtEvaluated"));
        lineEdit_RefreshAmbilihtEvaluated->setEnabled(true);
        lineEdit_RefreshAmbilihtEvaluated->setReadOnly(true);

        gridLayout->addWidget(lineEdit_RefreshAmbilihtEvaluated, 1, 1, 1, 1);

        label_hz = new QLabel(tabSoftwareOptions);
        label_hz->setObjectName(QString::fromUtf8("label_hz"));

        gridLayout->addWidget(label_hz, 1, 2, 1, 1);

        horizontalSliderHeight = new QSlider(tabSoftwareOptions);
        horizontalSliderHeight->setObjectName(QString::fromUtf8("horizontalSliderHeight"));
        horizontalSliderHeight->setMinimum(1);
        horizontalSliderHeight->setMaximum(800);
        horizontalSliderHeight->setValue(400);
        horizontalSliderHeight->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSliderHeight, 5, 0, 1, 3);

        checkBox_ShowPixelsAmbilight = new QCheckBox(tabSoftwareOptions);
        checkBox_ShowPixelsAmbilight->setObjectName(QString::fromUtf8("checkBox_ShowPixelsAmbilight"));
        checkBox_ShowPixelsAmbilight->setChecked(true);

        gridLayout->addWidget(checkBox_ShowPixelsAmbilight, 2, 0, 1, 3);

        spinBox_WidthAmbilight = new QSpinBox(tabSoftwareOptions);
        spinBox_WidthAmbilight->setObjectName(QString::fromUtf8("spinBox_WidthAmbilight"));
        spinBox_WidthAmbilight->setMinimum(1);
        spinBox_WidthAmbilight->setMaximum(1280);
        spinBox_WidthAmbilight->setValue(200);

        gridLayout->addWidget(spinBox_WidthAmbilight, 6, 1, 1, 1);

        horizontalSliderWidth = new QSlider(tabSoftwareOptions);
        horizontalSliderWidth->setObjectName(QString::fromUtf8("horizontalSliderWidth"));
        horizontalSliderWidth->setMinimum(1);
        horizontalSliderWidth->setMaximum(1280);
        horizontalSliderWidth->setValue(200);
        horizontalSliderWidth->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSliderWidth, 7, 0, 1, 3);


        verticalLayout_2->addLayout(gridLayout);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_WB_Red = new QLabel(tabSoftwareOptions);
        label_WB_Red->setObjectName(QString::fromUtf8("label_WB_Red"));
        label_WB_Red->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_WB_Red, 1, 0, 1, 1);

        label_WB_Green = new QLabel(tabSoftwareOptions);
        label_WB_Green->setObjectName(QString::fromUtf8("label_WB_Green"));
        label_WB_Green->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_WB_Green, 1, 1, 1, 1);

        label_WB_Blue = new QLabel(tabSoftwareOptions);
        label_WB_Blue->setObjectName(QString::fromUtf8("label_WB_Blue"));
        label_WB_Blue->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_WB_Blue, 1, 2, 1, 1);

        doubleSpinBox_WB_Red = new QDoubleSpinBox(tabSoftwareOptions);
        doubleSpinBox_WB_Red->setObjectName(QString::fromUtf8("doubleSpinBox_WB_Red"));
        doubleSpinBox_WB_Red->setDecimals(3);
        doubleSpinBox_WB_Red->setMinimum(0.2);
        doubleSpinBox_WB_Red->setMaximum(2);
        doubleSpinBox_WB_Red->setSingleStep(0.01);
        doubleSpinBox_WB_Red->setValue(1);

        gridLayout_3->addWidget(doubleSpinBox_WB_Red, 2, 0, 1, 1);

        doubleSpinBox_WB_Green = new QDoubleSpinBox(tabSoftwareOptions);
        doubleSpinBox_WB_Green->setObjectName(QString::fromUtf8("doubleSpinBox_WB_Green"));
        doubleSpinBox_WB_Green->setDecimals(3);
        doubleSpinBox_WB_Green->setMinimum(0.2);
        doubleSpinBox_WB_Green->setMaximum(2);
        doubleSpinBox_WB_Green->setSingleStep(0.01);
        doubleSpinBox_WB_Green->setValue(1);

        gridLayout_3->addWidget(doubleSpinBox_WB_Green, 2, 1, 1, 1);

        doubleSpinBox_WB_Blue = new QDoubleSpinBox(tabSoftwareOptions);
        doubleSpinBox_WB_Blue->setObjectName(QString::fromUtf8("doubleSpinBox_WB_Blue"));
        doubleSpinBox_WB_Blue->setDecimals(3);
        doubleSpinBox_WB_Blue->setMinimum(0.2);
        doubleSpinBox_WB_Blue->setMaximum(2);
        doubleSpinBox_WB_Blue->setSingleStep(0.01);
        doubleSpinBox_WB_Blue->setValue(1);

        gridLayout_3->addWidget(doubleSpinBox_WB_Blue, 2, 2, 1, 1);

        label_WhiteBalance = new QLabel(tabSoftwareOptions);
        label_WhiteBalance->setObjectName(QString::fromUtf8("label_WhiteBalance"));
        label_WhiteBalance->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_WhiteBalance, 0, 0, 1, 3);


        verticalLayout_2->addLayout(gridLayout_3);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        tabWidget->addTab(tabSoftwareOptions, QString());
        tabHardwareOptions = new QWidget();
        tabHardwareOptions->setObjectName(QString::fromUtf8("tabHardwareOptions"));
        verticalLayout_3 = new QVBoxLayout(tabHardwareOptions);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_HW_TimPrescaller = new QLabel(tabHardwareOptions);
        label_HW_TimPrescaller->setObjectName(QString::fromUtf8("label_HW_TimPrescaller"));

        gridLayout_2->addWidget(label_HW_TimPrescaller, 2, 0, 1, 1);

        comboBox_HW_Prescaller = new QComboBox(tabHardwareOptions);
        comboBox_HW_Prescaller->setObjectName(QString::fromUtf8("comboBox_HW_Prescaller"));

        gridLayout_2->addWidget(comboBox_HW_Prescaller, 2, 1, 1, 1);

        horizontalSlider_HW_Prescaller = new QSlider(tabHardwareOptions);
        horizontalSlider_HW_Prescaller->setObjectName(QString::fromUtf8("horizontalSlider_HW_Prescaller"));
        horizontalSlider_HW_Prescaller->setMinimum(0);
        horizontalSlider_HW_Prescaller->setMaximum(4);
        horizontalSlider_HW_Prescaller->setPageStep(1);
        horizontalSlider_HW_Prescaller->setValue(2);
        horizontalSlider_HW_Prescaller->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(horizontalSlider_HW_Prescaller, 3, 0, 1, 3);

        label_HW_TimerOCR = new QLabel(tabHardwareOptions);
        label_HW_TimerOCR->setObjectName(QString::fromUtf8("label_HW_TimerOCR"));

        gridLayout_2->addWidget(label_HW_TimerOCR, 4, 0, 1, 1);

        spinBox_HW_OCR = new QSpinBox(tabHardwareOptions);
        spinBox_HW_OCR->setObjectName(QString::fromUtf8("spinBox_HW_OCR"));
        spinBox_HW_OCR->setMinimum(1);
        spinBox_HW_OCR->setMaximum(255);
        spinBox_HW_OCR->setValue(7);

        gridLayout_2->addWidget(spinBox_HW_OCR, 4, 1, 1, 1);

        label_HW_Ticks = new QLabel(tabHardwareOptions);
        label_HW_Ticks->setObjectName(QString::fromUtf8("label_HW_Ticks"));

        gridLayout_2->addWidget(label_HW_Ticks, 4, 2, 1, 1);

        label_Reconnect = new QLabel(tabHardwareOptions);
        label_Reconnect->setObjectName(QString::fromUtf8("label_Reconnect"));

        gridLayout_2->addWidget(label_Reconnect, 0, 0, 1, 1);

        spinBox_ReconnectDelay = new QSpinBox(tabHardwareOptions);
        spinBox_ReconnectDelay->setObjectName(QString::fromUtf8("spinBox_ReconnectDelay"));
        spinBox_ReconnectDelay->setMinimum(1);
        spinBox_ReconnectDelay->setValue(2);

        gridLayout_2->addWidget(spinBox_ReconnectDelay, 0, 1, 1, 1);

        label_sec = new QLabel(tabHardwareOptions);
        label_sec->setObjectName(QString::fromUtf8("label_sec"));

        gridLayout_2->addWidget(label_sec, 0, 2, 1, 1);

        label_USBSendDataTimeout = new QLabel(tabHardwareOptions);
        label_USBSendDataTimeout->setObjectName(QString::fromUtf8("label_USBSendDataTimeout"));

        gridLayout_2->addWidget(label_USBSendDataTimeout, 1, 0, 1, 1);

        doubleSpinBoxUsbSendDataTimeout = new QDoubleSpinBox(tabHardwareOptions);
        doubleSpinBoxUsbSendDataTimeout->setObjectName(QString::fromUtf8("doubleSpinBoxUsbSendDataTimeout"));
        doubleSpinBoxUsbSendDataTimeout->setMinimum(0.2);
        doubleSpinBoxUsbSendDataTimeout->setMaximum(20);
        doubleSpinBoxUsbSendDataTimeout->setSingleStep(0.2);
        doubleSpinBoxUsbSendDataTimeout->setValue(1.2);

        gridLayout_2->addWidget(doubleSpinBoxUsbSendDataTimeout, 1, 1, 1, 1);

        label_sec_6 = new QLabel(tabHardwareOptions);
        label_sec_6->setObjectName(QString::fromUtf8("label_sec_6"));

        gridLayout_2->addWidget(label_sec_6, 1, 2, 1, 1);

        horizontalSlider_HW_OCR = new QSlider(tabHardwareOptions);
        horizontalSlider_HW_OCR->setObjectName(QString::fromUtf8("horizontalSlider_HW_OCR"));
        horizontalSlider_HW_OCR->setMinimum(1);
        horizontalSlider_HW_OCR->setMaximum(255);
        horizontalSlider_HW_OCR->setPageStep(1);
        horizontalSlider_HW_OCR->setValue(7);
        horizontalSlider_HW_OCR->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(horizontalSlider_HW_OCR, 5, 0, 1, 3);

        label_4 = new QLabel(tabHardwareOptions);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 10, 2, 1, 1);

        label = new QLabel(tabHardwareOptions);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 11, 0, 1, 3);

        label_2 = new QLabel(tabHardwareOptions);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 10, 0, 1, 1);

        label_HW_ColorDepth = new QLabel(tabHardwareOptions);
        label_HW_ColorDepth->setObjectName(QString::fromUtf8("label_HW_ColorDepth"));

        gridLayout_2->addWidget(label_HW_ColorDepth, 6, 0, 1, 1);

        horizontalSlider_HW_ColorDepth = new QSlider(tabHardwareOptions);
        horizontalSlider_HW_ColorDepth->setObjectName(QString::fromUtf8("horizontalSlider_HW_ColorDepth"));
        horizontalSlider_HW_ColorDepth->setMinimum(2);
        horizontalSlider_HW_ColorDepth->setMaximum(255);
        horizontalSlider_HW_ColorDepth->setPageStep(3);
        horizontalSlider_HW_ColorDepth->setValue(64);
        horizontalSlider_HW_ColorDepth->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(horizontalSlider_HW_ColorDepth, 7, 0, 1, 3);

        spinBox_HW_ColorDepth = new QSpinBox(tabHardwareOptions);
        spinBox_HW_ColorDepth->setObjectName(QString::fromUtf8("spinBox_HW_ColorDepth"));
        spinBox_HW_ColorDepth->setMinimum(2);
        spinBox_HW_ColorDepth->setMaximum(255);
        spinBox_HW_ColorDepth->setSingleStep(1);
        spinBox_HW_ColorDepth->setValue(64);

        gridLayout_2->addWidget(spinBox_HW_ColorDepth, 6, 1, 1, 1);

        label_HW_ColorDepth_counts = new QLabel(tabHardwareOptions);
        label_HW_ColorDepth_counts->setObjectName(QString::fromUtf8("label_HW_ColorDepth_counts"));

        gridLayout_2->addWidget(label_HW_ColorDepth_counts, 6, 2, 1, 1);

        lineEdit_PWM_Frequency = new QLineEdit(tabHardwareOptions);
        lineEdit_PWM_Frequency->setObjectName(QString::fromUtf8("lineEdit_PWM_Frequency"));
        lineEdit_PWM_Frequency->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_PWM_Frequency, 10, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout_2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        tabWidget->addTab(tabHardwareOptions, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(37, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pushButton_Close = new QPushButton(centralwidget);
        pushButton_Close->setObjectName(QString::fromUtf8("pushButton_Close"));

        horizontalLayout->addWidget(pushButton_Close);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralwidget);
        QWidget::setTabOrder(spinBox_UpdateDelay, spinBox_HeightAmbilight);
        QWidget::setTabOrder(spinBox_HeightAmbilight, spinBox_WidthAmbilight);

        retranslateUi(MainWindow);
        QObject::connect(horizontalSliderWidth, SIGNAL(valueChanged(int)), spinBox_WidthAmbilight, SLOT(setValue(int)));
        QObject::connect(spinBox_WidthAmbilight, SIGNAL(valueChanged(int)), horizontalSliderWidth, SLOT(setValue(int)));
        QObject::connect(horizontalSliderHeight, SIGNAL(valueChanged(int)), spinBox_HeightAmbilight, SLOT(setValue(int)));
        QObject::connect(spinBox_HeightAmbilight, SIGNAL(valueChanged(int)), horizontalSliderHeight, SLOT(setValue(int)));
        QObject::connect(comboBox_HW_Prescaller, SIGNAL(currentIndexChanged(int)), horizontalSlider_HW_Prescaller, SLOT(setValue(int)));
        QObject::connect(horizontalSlider_HW_Prescaller, SIGNAL(valueChanged(int)), comboBox_HW_Prescaller, SLOT(setCurrentIndex(int)));
        QObject::connect(horizontalSlider_HW_OCR, SIGNAL(valueChanged(int)), spinBox_HW_OCR, SLOT(setValue(int)));
        QObject::connect(spinBox_HW_OCR, SIGNAL(valueChanged(int)), horizontalSlider_HW_OCR, SLOT(setValue(int)));
        QObject::connect(horizontalSlider_HW_ColorDepth, SIGNAL(valueChanged(int)), spinBox_HW_ColorDepth, SLOT(setValue(int)));
        QObject::connect(spinBox_HW_ColorDepth, SIGNAL(valueChanged(int)), horizontalSlider_HW_ColorDepth, SLOT(setValue(int)));

        tabWidget->setCurrentIndex(0);
        comboBox_HW_Prescaller->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Ambilight USB - Settings", 0, QApplication::UnicodeUTF8));
        label_UpdateDelay->setText(QApplication::translate("MainWindow", "Refresh ambilight delay", 0, QApplication::UnicodeUTF8));
        label_ms->setText(QApplication::translate("MainWindow", "ms", 0, QApplication::UnicodeUTF8));
        label_height->setText(QApplication::translate("MainWindow", "Height", 0, QApplication::UnicodeUTF8));
        label_sec_4->setText(QApplication::translate("MainWindow", "pixels", 0, QApplication::UnicodeUTF8));
        label_width->setText(QApplication::translate("MainWindow", "Width", 0, QApplication::UnicodeUTF8));
        label_sec_5->setText(QApplication::translate("MainWindow", "pixels", 0, QApplication::UnicodeUTF8));
        label_UpdateDelayEval->setText(QApplication::translate("MainWindow", "Refresh ambilight evaluated", 0, QApplication::UnicodeUTF8));
        label_hz->setText(QApplication::translate("MainWindow", "Hz", 0, QApplication::UnicodeUTF8));
        checkBox_ShowPixelsAmbilight->setText(QApplication::translate("MainWindow", "Show pixels for ambilight", 0, QApplication::UnicodeUTF8));
        label_WB_Red->setText(QApplication::translate("MainWindow", "Red", 0, QApplication::UnicodeUTF8));
        label_WB_Green->setText(QApplication::translate("MainWindow", "Green", 0, QApplication::UnicodeUTF8));
        label_WB_Blue->setText(QApplication::translate("MainWindow", "Blue", 0, QApplication::UnicodeUTF8));
        label_WhiteBalance->setText(QApplication::translate("MainWindow", "White balance", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabSoftwareOptions), QApplication::translate("MainWindow", "Software options", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_HW_TimPrescaller->setToolTip(QApplication::translate("MainWindow", "Sets prescaller of the timer that generates PWM on HC595", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_HW_TimPrescaller->setText(QApplication::translate("MainWindow", "Timer prescaller", 0, QApplication::UnicodeUTF8));
        comboBox_HW_Prescaller->clear();
        comboBox_HW_Prescaller->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "64", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "256", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1024", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        label_HW_TimerOCR->setToolTip(QApplication::translate("MainWindow", "Sets output compate register (OCR) value of the timer that generates PWM on HC595", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_HW_TimerOCR->setText(QApplication::translate("MainWindow", "Timer OCR", 0, QApplication::UnicodeUTF8));
        label_HW_Ticks->setText(QApplication::translate("MainWindow", "ticks", 0, QApplication::UnicodeUTF8));
        label_Reconnect->setText(QApplication::translate("MainWindow", "Reconnect USB delay", 0, QApplication::UnicodeUTF8));
        label_sec->setText(QApplication::translate("MainWindow", "secs", 0, QApplication::UnicodeUTF8));
        label_USBSendDataTimeout->setText(QApplication::translate("MainWindow", "USB send data timeout", 0, QApplication::UnicodeUTF8));
        label_sec_6->setText(QApplication::translate("MainWindow", "secs", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Hz", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "System clock: 12 MHz", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Frequency of PWM generation:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_HW_ColorDepth->setToolTip(QApplication::translate("MainWindow", "Sets the number of levels in PWM generation for each channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_HW_ColorDepth->setText(QApplication::translate("MainWindow", "Color depth", 0, QApplication::UnicodeUTF8));
        label_HW_ColorDepth_counts->setText(QApplication::translate("MainWindow", "counts", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabHardwareOptions), QApplication::translate("MainWindow", "Hardware options", 0, QApplication::UnicodeUTF8));
        pushButton_Close->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
