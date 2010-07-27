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

#include "version.h"
#include "rectgetpixel.h"
#include "ambilightusb.h"

using namespace std;

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
    fprintf(stderr, "  --help   - show this help \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "sudo ./Ambilight    - try this if can't open device \n");
}

bool openLogFile()
{
    QString filePath = QDir::homePath() + "/.ambilight.log";

    cout << "Writing logs to: " << filePath.toStdString() << endl;

    QFile *logFile = new QFile(filePath);
    if(logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
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


int main(int argc, char **argv)
{
    if(!openLogFile()){
        cerr << "Log file didn't opened. Exit." << endl;
        return 2;
    }

    if(argc > 1){
        if(strcmp(argv[1], "--off") == 0){
            ambilightUsb ambilight_usb;
            ambilight_usb.offLeds();
            return 0;
        }else{
            showHelpMessage();
            return 1;
        }
    }


    Q_INIT_RESOURCE(icons);
    QApplication app(argc, argv);

    qInstallMsgHandler(messageOutput);
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Ambilight"),
                              QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow window;          /* Create MainWindow */
    window.setVisible(false);   /* And load to tray. */
    return app.exec();
}
