/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue Jul 27 23:26:59 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
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
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_Close;
    QSpacerItem *horizontalSpacer;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(376, 156);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Ambilight USB - \320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", 0, QApplication::UnicodeUTF8));
        label_UpdateDelay->setText(QApplication::translate("MainWindow", "\320\236\320\261\320\275\320\276\320\262\320\273\320\265\320\275\320\270\320\265 \320\277\320\276\320\264\321\201\320\262\320\265\321\202\320\272\320\270", 0, QApplication::UnicodeUTF8));
        label_hz->setText(QApplication::translate("MainWindow", "\320\223\321\206", 0, QApplication::UnicodeUTF8));
        label_Reconnect->setText(QApplication::translate("MainWindow", "\320\227\320\260\320\264\320\265\321\200\320\266\320\272\320\260 \320\277\320\265\321\200\320\265\320\277\320\276\320\264\320\272\320\273\321\216\321\207\320\265\320\275\320\270\321\217 \320\272 USB", 0, QApplication::UnicodeUTF8));
        label_sec->setText(QApplication::translate("MainWindow", "\321\201\320\265\320\272", 0, QApplication::UnicodeUTF8));
        pushButton_Close->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
