/*
 * main.c
 *
 *  Created on: 21.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 *          MCU: ATtiny44
 *        Clock: 12MHz
 */

#include <QtGui>
#include <iostream>
#include "mainwindow.h"

#include "settings.h"
#include "version.h"
#include "ambilightusb.h"

#include <sys/time.h>
#include "time.h"



using namespace std;

// Public visible object, #include "settings.h" for use it
QSettings *settings;

QTextStream logStream;

static void showHelpMessage()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Project : AmbilightUSB \n");
    fprintf(stderr, "Author  : brunql \n");
    fprintf(stderr, "Version : %s\n", VERSION_STR);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --off    - off leds \n");
    fprintf(stderr, "  --notr   - no translate (English version)\n");
    fprintf(stderr, "             use it if detect russian, but you want english \n");
    fprintf(stderr, "  --help   - show this help \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "sudo ./Ambilight    - try this if can't open device \n");
}

bool openLogFile(const QString & filePath)
{
    QFile *logFile = new QFile(filePath);
    QIODevice::OpenMode openFileAppendOrTruncateFlag = QIODevice::Append;
    QFileInfo info(filePath);
    if(info.size() > 1*1024*1024){
        cout << "Log file size > 1 Mb. I'm going to clear it. Now!\n";
        openFileAppendOrTruncateFlag = QIODevice::Truncate;
    }
    if(logFile->open(QIODevice::WriteOnly | openFileAppendOrTruncateFlag | QIODevice::Text)){
        logStream.setDevice(logFile);
        logStream << QDateTime::currentDateTime().date().toString("yyyy_MM_dd") << " ";
        logStream << QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") << " Ambilight v" << VERSION_STR << endl;
    }else{
        cerr << "Error open file for write: " << filePath.toStdString() << endl;
        return false;
    }
    return true;
}

void messageOutput(QtMsgType type, const char *msg)
{
    QString out = QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") + " ";
    switch (type) {
    case QtDebugMsg:
        cerr << "Debug: " << msg << endl;
        out.append("Debug: " + QString(msg));
        break;
    case QtWarningMsg:
        cerr << "Warning: " << msg << endl;
        out.append("Warning: " + QString(msg));
        break;
    case QtCriticalMsg:
        cerr << "Critical: " << msg << endl;
        out.append("Critical: " + QString(msg));
        break;
    case QtFatalMsg:
        cerr << "Fatal: " << msg << endl;
        cerr.flush();

        logStream << "Fatal: " << msg << endl;
        logStream.flush();

        abort();
    }
    logStream << out << endl;
    logStream.flush();
    cerr.flush();
}

void setDefaultSettingIfNotFound(const QString & name, const QVariant & value)
{
    if(!settings->contains(name)){
        qDebug() << "Settings:"<< name << "not found. Set it to default value: " << value.toString();
        settings->setValue(name, value);
    }
}

//
//  Check and/or initialize settings
//
void settingsInit()
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "brunql.dev", "AmbilightUSB");
    
    setDefaultSettingIfNotFound("RefreshAmbilightDelayMs",             REFRESH_AMBILIGHT_MS_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("ReconnectAmbilightUSBDelayMs",        RECONNECT_USB_MS_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("WidthAmbilight",                      WIDTH_AMBILIGHT_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("HeightAmbilight",                     HEIGHT_AMBILIGHT_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("IsAmbilightOn",                       IS_AMBILIGHT_ON_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("UsbSendDataTimeout",                  USB_SEND_DATA_TIMEOUT_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("WhiteBalanceCoefRed",                 WHITE_BALANCE_COEF_RED_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("WhiteBalanceCoefGreen",               WHITE_BALANCE_COEF_GREEN_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("WhiteBalanceCoefBlue",                WHITE_BALANCE_COEF_BLUE_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("HwTimerPrescallerIndex",              HW_TIMER_PRESCALLER_INDEX_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("HwTimerOCR",                          HW_TIMER_OCR_DEFAULT_VALUE);
    setDefaultSettingIfNotFound("HwColorDepth",                        HW_COLOR_DEPTH_DEFAULT_VALUE);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);    
    app.setApplicationVersion(VERSION_STR);

    QString logFilePath = QDir::homePath() + "/.ambilight.log";

    if(openLogFile(logFilePath)){
        cout << "Writing logs to '"<< logFilePath.toStdString() << "'";
    }else{
        cerr << "Log file '" << logFilePath.toStdString() << "' didn't opened. Exit." << endl;
        return 2;
    }

    QString locale = QLocale::system().name();
    if(argc > 1){
        if(strcmp(argv[1], "--off") == 0){
            AmbilightUsb ambilight_usb;
            ambilight_usb.offLeds();
            return 0;
        }else if(strcmp(argv[1], "--notr") == 0){
            locale = "en_EN";
        }else{
            showHelpMessage();
            return 1;
        }
    }

    Q_INIT_RESOURCE(res_ambilight);

    qInstallMsgHandler(messageOutput);
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, "AmbilightUSB",
                              "I couldn't detect any system tray on this system.");
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    QString pathToLocale = QString(":/translations/AmbilightUSB_") + locale;

    if(locale == "en_EN"){
        qWarning() << "Locale: " + locale;
    }else{
        QTranslator *translator = new QTranslator();
        if(translator->load(pathToLocale)){
            qDebug() << "Load translation for locale" << locale;
            app.installTranslator(translator);
        }else{
            qWarning() << "Locale:" << pathToLocale << "not found. Using defaults.";
        }        
    }

    settingsInit();

    MainWindow window;          /* Create MainWindow */
    window.setVisible(false);   /* And load to tray. */


    return app.exec();
}
