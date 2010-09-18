/********************************************************************************
** Form generated from reading UI file 'aboutdialog.ui'
**
** Created: Sat Sep 18 17:38:50 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_aboutDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label;
    QLabel *label_6;
    QLabel *label_5;
    QLabel *labelVersionSoftware;
    QLabel *labelVersionHardware;
    QLabel *label_4;
    QSpacerItem *verticalSpacer;
    QPushButton *pushButton;

    void setupUi(QDialog *aboutDialog)
    {
        if (aboutDialog->objectName().isEmpty())
            aboutDialog->setObjectName(QString::fromUtf8("aboutDialog"));
        aboutDialog->resize(418, 407);
        aboutDialog->setMinimumSize(QSize(400, 342));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/ambilight_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        aboutDialog->setWindowIcon(icon);
        aboutDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(aboutDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_2 = new QLabel(aboutDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMargin(10);

        verticalLayout->addWidget(label_2);

        label_3 = new QLabel(aboutDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);
        label_3->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        label_3->setWordWrap(true);
        label_3->setMargin(3);

        verticalLayout->addWidget(label_3);

        label = new QLabel(aboutDialog);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMinimumSize(QSize(0, 0));
        label->setAlignment(Qt::AlignJustify|Qt::AlignTop);
        label->setWordWrap(true);
        label->setMargin(3);

        verticalLayout->addWidget(label);

        label_6 = new QLabel(aboutDialog);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy);
        label_6->setFrameShape(QFrame::NoFrame);
        label_6->setFrameShadow(QFrame::Plain);
        label_6->setScaledContents(false);
        label_6->setMargin(3);
        label_6->setIndent(-1);

        verticalLayout->addWidget(label_6);

        label_5 = new QLabel(aboutDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMargin(3);

        verticalLayout->addWidget(label_5);

        labelVersionSoftware = new QLabel(aboutDialog);
        labelVersionSoftware->setObjectName(QString::fromUtf8("labelVersionSoftware"));
        labelVersionSoftware->setScaledContents(true);
        labelVersionSoftware->setMargin(3);

        verticalLayout->addWidget(labelVersionSoftware);

        labelVersionHardware = new QLabel(aboutDialog);
        labelVersionHardware->setObjectName(QString::fromUtf8("labelVersionHardware"));
        labelVersionHardware->setMargin(3);

        verticalLayout->addWidget(labelVersionHardware);

        label_4 = new QLabel(aboutDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout->addWidget(label_4);

        verticalSpacer = new QSpacerItem(50, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        pushButton = new QPushButton(aboutDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

#ifndef QT_NO_SHORTCUT
#endif // QT_NO_SHORTCUT

        retranslateUi(aboutDialog);
        QObject::connect(pushButton, SIGNAL(clicked()), aboutDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(aboutDialog);
    } // setupUi

    void retranslateUi(QDialog *aboutDialog)
    {
        aboutDialog->setWindowTitle(QApplication::translate("aboutDialog", "AmbilightUSB - About", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("aboutDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; font-weight:600;\">Ambilight USB</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("aboutDialog", "This project makes ambilight for you PC. Watching films now can be more fun. ", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("aboutDialog", "<b>Hardware:</b> ATtiny44, 74HC595 and 4 RGB leds. Using V-USB (HID Class device). ", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("aboutDialog", "<b>PC software:</b> C++, Qt, X11 (GUI tested in Ubuntu 10.04) ", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("aboutDialog", "<b>Author:</b> brunql", 0, QApplication::UnicodeUTF8));
        labelVersionSoftware->setText(QString());
        labelVersionHardware->setText(QString());
        label_4->setText(QApplication::translate("aboutDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt;\">Have fun!</span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:14pt;\"></p>\n"
"<p align=\"right\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">AmbilightUSB (<span style=\" font-style:italic;\">copyleft</span>) 2010 </p></body></html>", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("aboutDialog", "Ok, close it!", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class aboutDialog: public Ui_aboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
