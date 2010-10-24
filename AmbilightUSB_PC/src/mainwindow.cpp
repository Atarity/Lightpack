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

    ambilight_usb = new ambilightUsb();
    timer = new QTimer(this);

    loadSettingsToForm();


    // Show getPixelsRects then open settings
    ui->checkBox_ShowPixelsAmbilight->setChecked(true);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(timer, SIGNAL(timeout()), this, SLOT(timerForUsbPoll()));
    connect(ui->spinBox_UpdateDelay, SIGNAL(valueChanged(int)), this, SLOT(usbTimerDelayMsChange()));
    connect(ui->spinBox_ReconnectDelay, SIGNAL(valueChanged(int)), this, SLOT(usbTimerReconnectDelayMsChange()));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->checkBox_ShowPixelsAmbilight, SIGNAL(toggled(bool)), this, SLOT(settingsShowPixelsForAmbilight(bool)));
    connect(ui->spinBox_WidthAmbilight, SIGNAL(valueChanged(int)), this, SLOT(settingsWidthAmbilightChange()));
    connect(ui->spinBox_HeightAmbilight, SIGNAL(valueChanged(int)), this, SLOT(settingsHeightAmbilightChange()));
    connect(ui->doubleSpinBoxUsbSendDataTimeout, SIGNAL(valueChanged(double)), this, SLOT(settingsUsbSendDataTimeoutChange()));
    connect(ui->doubleSpinBox_WB_Red, SIGNAL(valueChanged(double)), this, SLOT(settingsWhiteBalanceRedChange()));
    connect(ui->doubleSpinBox_WB_Green, SIGNAL(valueChanged(double)), this, SLOT(settingsWhiteBalanceGreenChange()));
    connect(ui->doubleSpinBox_WB_Blue, SIGNAL(valueChanged(double)), this, SLOT(settingsWhiteBalanceBlueChange()));
    connect(ui->comboBox_HW_Prescaller, SIGNAL(currentIndexChanged(int)), this, SLOT(settingsHwTimerOptionsChange()));
    connect(ui->horizontalSlider_HW_Prescaller, SIGNAL(valueChanged(int)), this, SLOT(settingsHwTimerOptionsChange()));
    connect(ui->horizontalSlider_HW_OCR, SIGNAL(valueChanged(int)), this, SLOT(settingsHwTimerOptionsChange()));
    connect(ui->spinBox_HW_OCR, SIGNAL(valueChanged(int)), this, SLOT(settingsHwTimerOptionsChange()));

    usbTimerDelayMs = settings->value("RefreshAmbilightDelayMs").toInt();
    usbTimerReconnectDelayMs = settings->value("ReconnectAmbilightUSBDelayMs").toInt();


    isErrorState = false;
    isAmbilightOn = settings->value("IsAmbilightOn").toBool();
    if(isAmbilightOn){
        isAmbilightOn = false; // ambilightOn() check this and if this true, return
        ambilightOn();
    }else{
        isAmbilightOn = true; // ambilightOn() check this and if this false, return
        ambilightOff();
    }
    trayIcon->show();

    // Initialize limits of height and width
    ui->horizontalSliderWidth->setMaximum( QApplication::desktop()->width() / 2 );
    ui->spinBox_WidthAmbilight->setMaximum( QApplication::desktop()->width() / 2 );

    // 25px - default height of panels in Ubuntu 10.04
    ui->horizontalSliderHeight->setMaximum( QApplication::desktop()->height() / 2  - 25);
    ui->spinBox_HeightAmbilight->setMaximum( QApplication::desktop()->height() / 2 - 25);

    initGetPixelsRects();        
    settingsShowPixelsForAmbilight(false);

    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Window);    
}

void MainWindow::initGetPixelsRects()
{    
    for(int i=0; i<labelGetPixelsRects.count(); i++){
        labelGetPixelsRects[i]->close();
    }

    labelGetPixelsRects.clear();

    for(int i=0; i<LEDS_COUNT; i++){
        labelGetPixelsRects.append(new QLabel(this, Qt::FramelessWindowHint | Qt::SplashScreen));
        labelGetPixelsRects.last()->setFocusPolicy(Qt::NoFocus);
    }    

    updateSizesGetPixelsRects();
}

void MainWindow::updateSizesGetPixelsRects()
{
    int ambilight_width = settings->value("WidthAmbilight").toInt();
    int ambilight_height = settings->value("HeightAmbilight").toInt();
    int desktop_height = QApplication::desktop()->height();
    int desktop_width = QApplication::desktop()->width();


    for(int i=0; i<LEDS_COUNT; i++){
        labelGetPixelsRects[i]->setFixedWidth(ambilight_width);
        labelGetPixelsRects[i]->setFixedHeight(ambilight_height);
    }

    QPixmap rectPix(ambilight_width, ambilight_height);

    rectPix.fill(Qt::blue);
    labelGetPixelsRects[RIGHT_UP]->setPixmap(rectPix);
    labelGetPixelsRects[RIGHT_UP]->move(desktop_width - ambilight_width, desktop_height / 2 - ambilight_height);

    rectPix.fill(Qt::yellow);
    labelGetPixelsRects[RIGHT_DOWN]->setPixmap(rectPix);
    labelGetPixelsRects[RIGHT_DOWN]->move(desktop_width - ambilight_width, desktop_height / 2);

    rectPix.fill(Qt::red);
    labelGetPixelsRects[LEFT_UP]->setPixmap(rectPix);
    labelGetPixelsRects[LEFT_UP]->move(0, QApplication::desktop()->height() / 2 - ambilight_height);

    rectPix.fill(Qt::green);
    labelGetPixelsRects[LEFT_DOWN]->setPixmap(rectPix);
    labelGetPixelsRects[LEFT_DOWN]->move(0, QApplication::desktop()->height() / 2);

    this->activateWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
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
        settingsShowPixelsForAmbilight(false);
        hide();
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
    ambilight_usb->clearColorSave();

    if(isErrorState){
        timer->start(usbTimerDelayMs);
        isAmbilightOn = true;
    }else if(!isAmbilightOn){
        trayAmbilightOn();

        timer->start(usbTimerDelayMs);
        isAmbilightOn = true;
    }
    settings->setValue("IsAmbilightOn", isAmbilightOn);
}

void MainWindow::ambilightOff()
{
    if(isAmbilightOn){
        trayAmbilightOff();

        timer->stop();
        if(!isErrorState){
            if(ambilight_usb->deviceOpened()){
                ambilight_usb->offLeds();
            }
        }else{
            isErrorState = false;
        }
        isAmbilightOn = false;
    }
    settings->setValue("IsAmbilightOn", isAmbilightOn);
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
    QString hardwareVerison = ambilight_usb->hardwareVersion();
    aboutDialog *about = new aboutDialog(hardwareVerison, this);
    about->show();
}

void MainWindow::showSettings()
{
    this->move(QApplication::desktop()->width() / 2 - this->width() / 2,
            QApplication::desktop()->height() / 2 - this->height() / 2);
    settingsShowPixelsForAmbilight(ui->checkBox_ShowPixelsAmbilight->isChecked());
    this->show();
}

void MainWindow::settingsShowPixelsForAmbilight(bool state)
{
    if(state){        
        for(int i=0; i<LEDS_COUNT; i++){
            labelGetPixelsRects[i]->show();
        }                
    }else{
        for(int i=0; i<LEDS_COUNT; i++){
            labelGetPixelsRects[i]->hide();
        }
    }
}

void MainWindow::timerForUsbPoll()
{
    double updateResult_ms = ambilight_usb->updateColorsIfChanges();

    if(updateResult_ms > 0){
        if(isErrorState){
            isErrorState = false;
            
            trayAmbilightOn();

            qWarning() << "Ambilight USB. On state.";
            ambilight_usb->clearColorSave();
        }
        ui->lineEdit_RefreshAmbilihtEvaluated->setText(refreshAmbilightEvaluated(updateResult_ms));
        timer->start( usbTimerDelayMs );
    }else{
        if(!isErrorState){
            isErrorState = true;
            
            trayAmbilightError();

            qWarning() << "Ambilight USB. Error state.";
        }
        ui->lineEdit_RefreshAmbilihtEvaluated->setText("");
        timer->start( usbTimerReconnectDelayMs );
    }
}

QString MainWindow::refreshAmbilightEvaluated(double updateResult_ms)
{
    double hz = updateResult_ms + usbTimerDelayMs;

    if(hz != 0){
        hz = 1000 / hz;
    }

    return QString::number(hz,'f', 4); /* ms to hz */
}

void MainWindow::usbTimerDelayMsChange()
{
    int ms = ui->spinBox_UpdateDelay->value();
    settings->setValue("RefreshAmbilightDelayMs", ms);
    usbTimerDelayMs = ms;
}

void MainWindow::usbTimerReconnectDelayMsChange()
{
    int ms = ui->spinBox_ReconnectDelay->value() * 1000; /* sec to ms */
    settings->setValue("ReconnectAmbilightUSBDelayMs", ms);
    usbTimerReconnectDelayMs = ms;
}


void MainWindow::settingsWidthAmbilightChange()
{
    settings->setValue("WidthAmbilight",  ui->spinBox_WidthAmbilight->value());
    ambilight_usb->readSettings();
    updateSizesGetPixelsRects();
}

void MainWindow::settingsHeightAmbilightChange()
{
    settings->setValue("HeightAmbilight", ui->spinBox_HeightAmbilight->value() );
    ambilight_usb->readSettings();
    updateSizesGetPixelsRects();
}

void MainWindow::settingsUsbSendDataTimeoutChange()
{
    settings->setValue("UsbSendDataTimeout", (int)(ui->doubleSpinBoxUsbSendDataTimeout->value() * 1000));
    ambilight_usb->readSettings();
}

void MainWindow::settingsWhiteBalanceRedChange()
{
    settings->setValue("WhiteBalanceCoefRed", ui->doubleSpinBox_WB_Red->value());
    ambilight_usb->readSettings();
}

void MainWindow::settingsWhiteBalanceGreenChange()
{
    settings->setValue("WhiteBalanceCoefGreen", ui->doubleSpinBox_WB_Green->value());
    ambilight_usb->readSettings();
}

void MainWindow::settingsWhiteBalanceBlueChange()
{
    settings->setValue("WhiteBalanceCoefBlue", ui->doubleSpinBox_WB_Blue->value());
    ambilight_usb->readSettings();
}


// Send settings to device and update evaluated Hz of the PWM generation
void MainWindow::settingsHwTimerOptionsChange()
{
    int timerPrescallerIndex = ui->comboBox_HW_Prescaller->currentIndex();
    int timerOutputCompareRegValue = ui->spinBox_HW_OCR->value();
    settings->setValue("HwTimerPrescallerIndex", timerPrescallerIndex);
    settings->setValue("HwTimerOCR", timerOutputCompareRegValue);
    ambilight_usb->setTimerOptions(timerPrescallerIndex, timerOutputCompareRegValue);

    // TODO: add PWM level max value to settings
    double pwmFreq = 12000000 / 64; // 64 - PWM level max value;
    switch(timerPrescallerIndex){
        case CMD_SET_PRESCALLER_1:      break;
        case CMD_SET_PRESCALLER_8:      pwmFreq /= 8; break;
        case CMD_SET_PRESCALLER_64:     pwmFreq /= 64;break;
        case CMD_SET_PRESCALLER_256:    pwmFreq /= 256;break;
        case CMD_SET_PRESCALLER_1024:   pwmFreq /= 1024;break;
        default: qWarning() << "bad value of 'timerPrescallerIndex' =" << timerPrescallerIndex; break;
    }
    pwmFreq /= timerOutputCompareRegValue;

    ui->label_PWM_Frequency->setText(QString::number(pwmFreq,'g',3));
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
}

void MainWindow::loadSettingsToForm()
{
    ui->spinBox_UpdateDelay->setValue( settings->value("RefreshAmbilightDelayMs").toInt() );
    ui->spinBox_ReconnectDelay->setValue( settings->value("ReconnectAmbilightUSBDelayMs").toInt() / 1000 /* ms to sec */ );

    ui->spinBox_WidthAmbilight->setValue( settings->value("WidthAmbilight").toInt() );
    ui->spinBox_HeightAmbilight->setValue( settings->value("HeightAmbilight").toInt() );

    ui->doubleSpinBoxUsbSendDataTimeout->setValue( settings->value("UsbSendDataTimeout").toInt() / 1000.0 /* ms to sec */ );

    ui->spinBox_HW_OCR->setValue( settings->value("HwTimerOCR").toInt());
    ui->comboBox_HW_Prescaller->setCurrentIndex( settings->value("HwTimerPrescallerIndex").toInt());

    ui->doubleSpinBox_WB_Red->setValue(settings->value("WhiteBalanceCoefRed").toDouble());
    ui->doubleSpinBox_WB_Green->setValue(settings->value("WhiteBalanceCoefGreen").toDouble());
    ui->doubleSpinBox_WB_Blue->setValue(settings->value("WhiteBalanceCoefBlue").toDouble());
}
