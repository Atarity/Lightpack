/*
 * mainwindow.cpp
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createTrayIcon();

    qDebug() << "MainWindow(): new AmbilightUsb(this)";
    ambilightUsb = new AmbilightUsb(this);

    qDebug() << "MainWindow(): new GrabDesktopWindowLeds(this)";
    grabDesktopWindowLeds = new GrabDesktopWindowLeds(this);

    qDebug() << "MainWindow(): loadSettingsToMainWindow()";
    loadSettingsToMainWindow();
    
    qDebug() << "MainWindow(): connectSignalsSlots()";
    connectSignalsSlots();


    // Initialize limits of height and width
    ui->horizontalSliderWidth->setMaximum( QApplication::desktop()->width() / 2 );
    ui->spinBox_WidthAmbilight->setMaximum( QApplication::desktop()->width() / 2 );

    // 25px - default height of panels in Ubuntu 10.04
    ui->horizontalSliderHeight->setMaximum( QApplication::desktop()->height() / 2  - 25);
    ui->spinBox_HeightAmbilight->setMaximum( QApplication::desktop()->height() / 2 - 25);

        
    isErrorState = false;
    isAmbilightOn = settings->value("IsAmbilightOn").toBool();
    if(isAmbilightOn){
        isAmbilightOn = false; // ambilightOn() check this and if this true, return
        qDebug() << "MainWindow(): ambilightOn()";
        ambilightOn();
    }else{
        isAmbilightOn = true; // ambilightOn() check this and if this false, return
        qDebug() << "MainWindow(): ambilightOff()";
        ambilightOff();
    }


    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Window);    

    qDebug() << "MainWindow(): initialized";
}

void MainWindow::connectSignalsSlots()
{
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));

    // TODO: ambilightUsb slot setUsbReconnectDelay
    //connect(ui->spinBox_ReconnectDelay, SIGNAL(valueChanged(int)), ambilightUsb, SLOT(setUsbReconnectDelay(int)));    
    connect(ui->doubleSpinBoxUsbSendDataTimeout, SIGNAL(valueChanged(double)), ambilightUsb, SLOT(setUsbSendDataTimeoutMs(double)));

    // Connect to grabDesktopWindowLeds
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightRefreshDelayMs(int)));
    connect(ui->checkBox_ShowPixelsAmbilight, SIGNAL(toggled(bool)), grabDesktopWindowLeds, SLOT(setVisibleGrabPixelsRects(bool)));
    connect(ui->spinBox_WidthAmbilight, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightWidth(int)));
    connect(ui->spinBox_HeightAmbilight, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightHeight(int)));
    connect(ui->doubleSpinBox_WB_Red, SIGNAL(valueChanged(double)), grabDesktopWindowLeds, SLOT(setAmbilightWhiteBalanceR(double)));
    connect(ui->doubleSpinBox_WB_Green, SIGNAL(valueChanged(double)), grabDesktopWindowLeds, SLOT(setAmbilightWhiteBalanceG(double)));
    connect(ui->doubleSpinBox_WB_Blue, SIGNAL(valueChanged(double)), grabDesktopWindowLeds, SLOT(setAmbilightWhiteBalanceB(double)));
    connect(ui->horizontalSlider_HW_ColorDepth, SIGNAL(valueChanged(int)), grabDesktopWindowLeds, SLOT(setAmbilightColorDepth(int)));    

    // Connect grabDesktopWindowLeds with ambilightUsb
    connect(grabDesktopWindowLeds, SIGNAL(updateLedsColors(int[LEDS_COUNT][3])), ambilightUsb, SLOT(updateColors(int[LEDS_COUNT][3])));

    // Software options
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->checkBox_ShowPixelsAmbilight, SIGNAL(toggled(bool)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->spinBox_WidthAmbilight, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->spinBox_HeightAmbilight, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->doubleSpinBox_WB_Red, SIGNAL(valueChanged(double)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->doubleSpinBox_WB_Green, SIGNAL(valueChanged(double)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->doubleSpinBox_WB_Blue, SIGNAL(valueChanged(double)), this, SLOT(settingsSoftwareOptionsChange()));
    // Hardware options
    connect(ui->horizontalSlider_HW_ColorDepth, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareOptionsChange()));
    connect(ui->horizontalSlider_HW_Prescaller, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareOptionsChange()));
    connect(ui->horizontalSlider_HW_OCR, SIGNAL(valueChanged(int)), this, SLOT(settingsHardwareOptionsChange()));
    // Software/Hardware options
    connect(ui->spinBox_ReconnectDelay, SIGNAL(valueChanged(int)), this, SLOT(settingsSoftwareOptionsChange()));
    connect(ui->doubleSpinBoxUsbSendDataTimeout, SIGNAL(valueChanged(double)), this, SLOT(settingsSoftwareOptionsChange()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete onAmbilightAction;
    delete offAmbilightAction;
    delete settingsAction;
    delete aboutAction;
    delete quitAction;

    delete trayIcon;
    delete trayIconMenu;

    delete ambilightUsb;
    delete grabDesktopWindowLeds;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        grabDesktopWindowLeds->setVisibleGrabPixelsRects(false);
        this->hide();
        event->ignore();
    }
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if(isAmbilightOn){
            ambilightOff();
        }else{
            ambilightOn();
        }
        break;
    case QSystemTrayIcon::MiddleClick:
    case QSystemTrayIcon::Trigger:
    default: break;
    }
}

void MainWindow::ambilightOn()
{
    grabDesktopWindowLeds->clearColors();

    if(isErrorState == false && isAmbilightOn == false){
        trayAmbilightOn();
    }
    isAmbilightOn = true;
    settings->setValue("IsAmbilightOn", isAmbilightOn);
    grabDesktopWindowLeds->setAmbilightOn(isAmbilightOn);
}

void MainWindow::ambilightOff()
{
    if(isAmbilightOn){
        trayAmbilightOff();

        if(!isErrorState){
            if(ambilightUsb->deviceOpened()){
                ambilightUsb->offLeds();
            }
        }else{
            isErrorState = false;
        }
        isAmbilightOn = false;
    }
    settings->setValue("IsAmbilightOn", isAmbilightOn);
    grabDesktopWindowLeds->setAmbilightOn(isAmbilightOn);
}

void MainWindow::trayAmbilightOn()
{
    trayIcon->setIcon(QIcon(":/icons/on.png"));
    trayIcon->setToolTip(tr("Ambilight USB. On state."));
}

void MainWindow::trayAmbilightOff()
{
    trayIcon->setIcon(QIcon(":/icons/off.png"));
    trayIcon->setToolTip(tr("Ambilight USB. Off state."));
}

void MainWindow::trayAmbilightError()
{
    trayIcon->setIcon(QIcon(":/icons/error.png"));
    trayIcon->setToolTip(tr("Ambilight USB. Error state."));
}



void MainWindow::showAbout()
{
    QString hardwareVerison = ambilightUsb->hardwareVersion();
    aboutDialog *about = new aboutDialog(hardwareVerison, this);
    about->show();
}

void MainWindow::showSettings()
{
    this->move(QApplication::desktop()->width() / 2 - this->width() / 2,
            QApplication::desktop()->height() / 2 - this->height() / 2);
    grabDesktopWindowLeds->setVisibleGrabPixelsRects(ui->checkBox_ShowPixelsAmbilight->isChecked());
    this->show();
}

//void MainWindow::timerForUsbPoll()
//{
//    if(updateResult_ms > 0){
//        if(isErrorState){
//            isErrorState = false;
//
//            trayAmbilightOn();
//
//            qWarning() << "Ambilight USB. On state.";
//            ambilight_usb->clearColorSave();
//        }
//        ui->lineEdit_RefreshAmbilihtEvaluated->setText(refreshAmbilightEvaluated(updateResult_ms));
//        timer->start( usbTimerDelayMs );
//    }else{
//        if(!isErrorState){
//            isErrorState = true;
//
//            trayAmbilightError();
//
//            qWarning() << "Ambilight USB. Error state.";
//        }
//        ui->lineEdit_RefreshAmbilihtEvaluated->setText("");
//        timer->start( usbTimerReconnectDelayMs );
//    }
//}

QString MainWindow::refreshAmbilightEvaluated(double updateResultMs)
{    
    int usbTimerDelayMs = ui->spinBox_UpdateDelay->value();
    double secs = (updateResultMs + usbTimerDelayMs) / 1000;
    double hz = 0;

    if(secs != 0){
        hz = 1 / secs;
    }

    return QString::number(hz,'f', 4); /* ms to hz */
}

void MainWindow::settingsSoftwareOptionsChange()
{
    settings->setValue("RefreshAmbilightDelayMs", ui->spinBox_UpdateDelay->value());
    settings->setValue("ReconnectAmbilightUSBDelayMs", ui->spinBox_ReconnectDelay->value() * 1000 /* sec to ms */);
    settings->setValue("WidthAmbilight",  ui->spinBox_WidthAmbilight->value());
    settings->setValue("HeightAmbilight", ui->spinBox_HeightAmbilight->value());
    settings->setValue("UsbSendDataTimeout", (int)(ui->doubleSpinBoxUsbSendDataTimeout->value() * 1000));
    settings->setValue("WhiteBalanceCoefRed", ui->doubleSpinBox_WB_Red->value());
    settings->setValue("WhiteBalanceCoefGreen", ui->doubleSpinBox_WB_Green->value());
    settings->setValue("WhiteBalanceCoefBlue", ui->doubleSpinBox_WB_Blue->value());
}

// Send settings to device and update evaluated Hz of the PWM generation
void MainWindow::settingsHardwareOptionsChange()
{
    int timerPrescallerIndex = ui->comboBox_HW_Prescaller->currentIndex();
    int timerOutputCompareRegValue = ui->spinBox_HW_OCR->value();
    int colorDepth = ui->horizontalSlider_HW_ColorDepth->value();

    settings->setValue("HwTimerPrescallerIndex", timerPrescallerIndex);
    settings->setValue("HwTimerOCR", timerOutputCompareRegValue);
    settings->setValue("HwColorDepth", colorDepth);

    if(colorDepth == 0) {
        qWarning() << "void MainWindow::settingsHardwareOptionsChange() Magic! colorDepth == 0";
        return;
    }
    double pwmFreq = 12000000 / colorDepth; // colorDepth - PWM level max value;

    switch(timerPrescallerIndex){
    case CMD_SET_PRESCALLER_1:      break;
    case CMD_SET_PRESCALLER_8:      pwmFreq /= 8; break;
    case CMD_SET_PRESCALLER_64:     pwmFreq /= 64;break;
    case CMD_SET_PRESCALLER_256:    pwmFreq /= 256;break;
    case CMD_SET_PRESCALLER_1024:   pwmFreq /= 1024;break;
    default: qWarning() << "bad value of 'timerPrescallerIndex' =" << timerPrescallerIndex; break;
    }
    pwmFreq /= timerOutputCompareRegValue;

    ui->lineEdit_PWM_Frequency->setText(QString::number(pwmFreq,'g',3));
}



void MainWindow::createActions()
{
    onAmbilightAction = new QAction(tr("&On ambilight"), this);
    connect(onAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOn()));

    offAmbilightAction = new QAction(tr("O&ff ambilight"), this);
    connect(offAmbilightAction, SIGNAL(triggered()), this, SLOT(ambilightOff()));

    settingsAction = new QAction(tr("&Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(onAmbilightAction);
    trayIconMenu->addAction(offAmbilightAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icons/off.png"));
    trayIcon->show();
}

void MainWindow::loadSettingsToMainWindow()
{
    ui->spinBox_UpdateDelay->setValue( settings->value("RefreshAmbilightDelayMs").toInt() );
    ui->spinBox_ReconnectDelay->setValue( settings->value("ReconnectAmbilightUSBDelayMs").toInt() / 1000 /* ms to sec */ );

    ui->spinBox_WidthAmbilight->setValue( settings->value("WidthAmbilight").toInt() );
    ui->spinBox_HeightAmbilight->setValue( settings->value("HeightAmbilight").toInt() );

    ui->doubleSpinBoxUsbSendDataTimeout->setValue( settings->value("UsbSendDataTimeout").toInt() / 1000.0 /* ms to sec */ );

    ui->horizontalSlider_HW_OCR->setValue( settings->value("HwTimerOCR").toInt());
    ui->horizontalSlider_HW_Prescaller->setValue( settings->value("HwTimerPrescallerIndex").toInt());
    ui->horizontalSlider_HW_ColorDepth->setValue( settings->value("HwColorDepth").toInt());

    ui->doubleSpinBox_WB_Red->setValue(settings->value("WhiteBalanceCoefRed").toDouble());
    ui->doubleSpinBox_WB_Green->setValue(settings->value("WhiteBalanceCoefGreen").toDouble());
    ui->doubleSpinBox_WB_Blue->setValue(settings->value("WhiteBalanceCoefBlue").toDouble());

//    settingsHardwareOptionsChange(); // eval PWM generation frequency and show it in settings
}
