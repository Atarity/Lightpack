/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed Aug 4 01:40:36 2010
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
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
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
    QLabel *label_hz;
    QLabel *label_Reconnect;
    QSpinBox *spinBox_ReconnectDelay;
    QLabel *label_sec;
    QLabel *label_x_step;
    QLabel *label_sec_2;
    QSpinBox *spinBox_StepY;
    QLabel *label_sec_3;
    QSpinBox *spinBox_StepX;
    QLabel *label_y_step;
    QLabel *label_height;
    QSpinBox *spinBox_HeightAmbilight;
    QLabel *label_sec_4;
    QLabel *label_width;
    QSpinBox *spinBox_WidthAmbilight;
    QLabel *label_sec_5;
    QCheckBox *checkBox_ShowPixelsAmbilight;
    QCheckBox *checkBox_ShowPixelsTransparentBackground;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_Close;
    QSpacerItem *horizontalSpacer;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(318, 332);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
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
        spinBox_UpdateDelay->setMinimum(1);
        spinBox_UpdateDelay->setMaximum(1000);
        spinBox_UpdateDelay->setValue(20);

        gridLayout->addWidget(spinBox_UpdateDelay, 0, 1, 1, 1);

        label_hz = new QLabel(centralwidget);
        label_hz->setObjectName(QString::fromUtf8("label_hz"));

        gridLayout->addWidget(label_hz, 0, 2, 1, 1);

        label_Reconnect = new QLabel(centralwidget);
        label_Reconnect->setObjectName(QString::fromUtf8("label_Reconnect"));

        gridLayout->addWidget(label_Reconnect, 1, 0, 1, 1);

        spinBox_ReconnectDelay = new QSpinBox(centralwidget);
        spinBox_ReconnectDelay->setObjectName(QString::fromUtf8("spinBox_ReconnectDelay"));
        spinBox_ReconnectDelay->setMinimum(1);
        spinBox_ReconnectDelay->setValue(2);

        gridLayout->addWidget(spinBox_ReconnectDelay, 1, 1, 1, 1);

        label_sec = new QLabel(centralwidget);
        label_sec->setObjectName(QString::fromUtf8("label_sec"));

        gridLayout->addWidget(label_sec, 1, 2, 1, 1);

        label_x_step = new QLabel(centralwidget);
        label_x_step->setObjectName(QString::fromUtf8("label_x_step"));

        gridLayout->addWidget(label_x_step, 4, 0, 1, 1);

        label_sec_2 = new QLabel(centralwidget);
        label_sec_2->setObjectName(QString::fromUtf8("label_sec_2"));

        gridLayout->addWidget(label_sec_2, 4, 2, 1, 1);

        spinBox_StepY = new QSpinBox(centralwidget);
        spinBox_StepY->setObjectName(QString::fromUtf8("spinBox_StepY"));
        spinBox_StepY->setMinimum(1);
        spinBox_StepY->setValue(20);

        gridLayout->addWidget(spinBox_StepY, 5, 1, 1, 1);

        label_sec_3 = new QLabel(centralwidget);
        label_sec_3->setObjectName(QString::fromUtf8("label_sec_3"));

        gridLayout->addWidget(label_sec_3, 5, 2, 1, 1);

        spinBox_StepX = new QSpinBox(centralwidget);
        spinBox_StepX->setObjectName(QString::fromUtf8("spinBox_StepX"));
        spinBox_StepX->setMinimum(1);
        spinBox_StepX->setValue(20);

        gridLayout->addWidget(spinBox_StepX, 4, 1, 1, 1);

        label_y_step = new QLabel(centralwidget);
        label_y_step->setObjectName(QString::fromUtf8("label_y_step"));

        gridLayout->addWidget(label_y_step, 5, 0, 1, 1);

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

        gridLayout->addWidget(label_width, 7, 0, 1, 1);

        spinBox_WidthAmbilight = new QSpinBox(centralwidget);
        spinBox_WidthAmbilight->setObjectName(QString::fromUtf8("spinBox_WidthAmbilight"));
        spinBox_WidthAmbilight->setMinimum(1);
        spinBox_WidthAmbilight->setMaximum(1280);
        spinBox_WidthAmbilight->setValue(200);

        gridLayout->addWidget(spinBox_WidthAmbilight, 7, 1, 1, 1);

        label_sec_5 = new QLabel(centralwidget);
        label_sec_5->setObjectName(QString::fromUtf8("label_sec_5"));

        gridLayout->addWidget(label_sec_5, 7, 2, 1, 1);

        checkBox_ShowPixelsAmbilight = new QCheckBox(centralwidget);
        checkBox_ShowPixelsAmbilight->setObjectName(QString::fromUtf8("checkBox_ShowPixelsAmbilight"));

        gridLayout->addWidget(checkBox_ShowPixelsAmbilight, 2, 0, 1, 3);

        checkBox_ShowPixelsTransparentBackground = new QCheckBox(centralwidget);
        checkBox_ShowPixelsTransparentBackground->setObjectName(QString::fromUtf8("checkBox_ShowPixelsTransparentBackground"));

        gridLayout->addWidget(checkBox_ShowPixelsTransparentBackground, 3, 0, 1, 3);


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

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Ambilight USB - Settings", 0, QApplication::UnicodeUTF8));
        label_UpdateDelay->setText(QApplication::translate("MainWindow", "Refresh ambilight", 0, QApplication::UnicodeUTF8));
        label_hz->setText(QApplication::translate("MainWindow", "Hz", 0, QApplication::UnicodeUTF8));
        label_Reconnect->setText(QApplication::translate("MainWindow", "Reconnect USB delay", 0, QApplication::UnicodeUTF8));
        label_sec->setText(QApplication::translate("MainWindow", "secs", 0, QApplication::UnicodeUTF8));
        label_x_step->setText(QApplication::translate("MainWindow", "Step X", 0, QApplication::UnicodeUTF8));
        label_sec_2->setText(QApplication::translate("MainWindow", "pixels", 0, QApplication::UnicodeUTF8));
        label_sec_3->setText(QApplication::translate("MainWindow", "pixels", 0, QApplication::UnicodeUTF8));
        label_y_step->setText(QApplication::translate("MainWindow", "Step Y", 0, QApplication::UnicodeUTF8));
        label_height->setText(QApplication::translate("MainWindow", "Height", 0, QApplication::UnicodeUTF8));
        label_sec_4->setText(QApplication::translate("MainWindow", "pixels", 0, QApplication::UnicodeUTF8));
        label_width->setText(QApplication::translate("MainWindow", "Width", 0, QApplication::UnicodeUTF8));
        label_sec_5->setText(QApplication::translate("MainWindow", "pixels", 0, QApplication::UnicodeUTF8));
        checkBox_ShowPixelsAmbilight->setText(QApplication::translate("MainWindow", "Show pixels for ambilight", 0, QApplication::UnicodeUTF8));
        checkBox_ShowPixelsTransparentBackground->setText(QApplication::translate("MainWindow", "Show pixels with transparent background", 0, QApplication::UnicodeUTF8));
        pushButton_Close->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
