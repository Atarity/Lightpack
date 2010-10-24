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

bool openLogFile()
{
    QString filePath = QDir::homePath() + "/.ambilight.log";

    cout << "Writing logs to: " << filePath.toStdString() << endl;

    QFile *logFile = new QFile(filePath);
    QIODevice::OpenMode openFileAppendOrTruncateFlag = QIODevice::Append;
    QFileInfo info(filePath);
    if(info.size() > 1*1024*1024){
        fprintf(stderr, "Log file size > 1 Mb. I'm going to clear it. Now!\n");
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
        logStream << "Fatal: " << msg << endl;
        logStream.flush();
        abort();
    }
    logStream << out << endl;
    logStream.flush();
}

//
//  Check and/or initialize settings
//
void settingsInit()
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "brunql.dev", "AmbilightUSB");
    if(settings->value("RefreshAmbilightDelayMs") == QVariant()){
        qDebug() << "Settings: 'RefreshAmbilightDelayMs' not found. Set it to default value:"
                << REFRESH_AMBILIGHT_MS_DEFAULT_VALUE;
        settings->setValue("RefreshAmbilightDelayMs", REFRESH_AMBILIGHT_MS_DEFAULT_VALUE);
    }
    if(settings->value("ReconnectAmbilightUSBDelayMs") == QVariant()){
        qDebug() << "Settings: 'ReconnectAmbilightUSBDelayMs' not found. Set it to default value:"
                << RECONNECT_USB_MS_DEFAULT_VALUE;
        settings->setValue("ReconnectAmbilightUSBDelayMs", RECONNECT_USB_MS_DEFAULT_VALUE);
    }
    if(settings->value("WidthAmbilight") == QVariant()){
        qDebug() << "Settings: 'WidthAmbilight' not found. Set it to default value:"
                << WIDTH_AMBILIGHT_DEFAULT_VALUE;
        settings->setValue("WidthAmbilight", WIDTH_AMBILIGHT_DEFAULT_VALUE);
    }
    if(settings->value("HeightAmbilight") == QVariant()){
        qDebug() << "Settings: 'HeightAmbilight' not found. Set it to default value:"
                << HEIGHT_AMBILIGHT_DEFAULT_VALUE;
        settings->setValue("HeightAmbilight", HEIGHT_AMBILIGHT_DEFAULT_VALUE);
    }
    if(settings->value("IsAmbilightOn") == QVariant()){
        qDebug() << "Settings: 'IsAmbilightOn' not found. Set it to default value:"
                << IS_AMBILIGHT_ON_DEFAULT_VALUE;
        settings->setValue("IsAmbilightOn", IS_AMBILIGHT_ON_DEFAULT_VALUE);
    }
    if(settings->value("UsbSendDataTimeout") == QVariant()){
        qDebug() << "Settings: 'UsbSendDataTimeout' not found. Set it to default value:"
                << USB_SEND_DATA_TIMEOUT;
        settings->setValue("UsbSendDataTimeout", USB_SEND_DATA_TIMEOUT);
    }

}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if(!openLogFile()){
        cerr << "Log file didn't opened. Exit." << endl;
        return 2;
    }

    QString locale = QLocale::system().name();
    if(argc > 1){
        if(strcmp(argv[1], "--off") == 0){
            ambilightUsb ambilight_usb;
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

    QString pathToLocale = QString(":/translations/Ambilight_") + locale;    

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
