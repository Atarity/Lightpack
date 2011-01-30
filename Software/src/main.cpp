/*
 * main.cpp
 *
 *  Created on: 21.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <QtGui>
#include <iostream>
#include "mainwindow.h"

#include "settings.h"
#include "version.h"
#include "ambilightusb.h"
#include "desktop.h"

#include <sys/time.h>
#include "time.h"




using namespace std;

// Public visible object, #include "settings.h" for use it
QSettings *settings;

QTextStream logStream;

// Make it global for access in messageOutput() for returning logs to window
MainWindow * window;

QString logWhileWindowNotInitialized = "";

static void showHelpMessage()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Project : Lightpack \n");
    fprintf(stderr, "Author  : brunql \n");
    fprintf(stderr, "Version : %s\n", VERSION_STR);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --off    - off leds \n");
    fprintf(stderr, "  --notr   - no translate (English version)\n");
    fprintf(stderr, "             use it if detect russian, but you want english \n");
    fprintf(stderr, "  --help   - show this help \n");
    fprintf(stderr, "\n");
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
        logStream << QDateTime::currentDateTime().time().toString("hh:mm:ss:zzz") << " Lightpack v" << VERSION_STR << endl;
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
        out.append("Debug: " + QString(msg));        
        cout << out.toStdString() << endl;
        break;
    case QtWarningMsg:
        out.append("Warning: " + QString(msg));
        cerr << out.toStdString() << endl;
        break;
    case QtCriticalMsg:
        out.append("Critical: " + QString(msg));
        cerr << out.toStdString() << endl;
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
    cout.flush();

    if(window != NULL){
        if(logWhileWindowNotInitialized != ""){
            logWhileWindowNotInitialized.truncate( logWhileWindowNotInitialized.length() - 1);
            window->appendLogsLine(logWhileWindowNotInitialized);
            logWhileWindowNotInitialized = "";
        }
        window->appendLogsLine(out);
    }else{
        logWhileWindowNotInitialized.append(out + "\n");
    }
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
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "brunql.dev", "Lightpack");
    qDebug() << "Settings file: " << settings->fileName();
    
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
    setDefaultSettingIfNotFound("HwChangeColorsDelay",                 HW_CHANGE_COLORS_SMOOTH_DELAY_VALUE);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);    
    app.setApplicationVersion(VERSION_STR);

    QString logFilePath = QDir::homePath() + "/.Lightpack.log";

    if(openLogFile(logFilePath)){
        cout << "Logs file: " << logFilePath.toStdString() << endl;
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

    // Initialize 'settings' variable, new settings initializes with default values
    settingsInit();

    Q_INIT_RESOURCE(LightpackResources);

    qInstallMsgHandler(messageOutput);
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, "Lightpack",
                              "I couldn't detect any system tray on this system.");
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    QString pathToLocale = QString(":/translations/") + locale;

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

    Desktop desktop; // Call constructor, for fill sizes, after initialize QApplication

    window = new MainWindow();   /* Create MainWindow */
    window->setVisible(false);   /* And load to tray. */

    // Don't touch me!!!
    qDebug() << "call app.exec();";

    return app.exec();
}
