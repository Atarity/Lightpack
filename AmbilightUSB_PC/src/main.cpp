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

using namespace std;

QTextStream logStream;


static void showHelpMessage(char *myName)
{
    fprintf(stderr, "Project : AmbilightUSB \n");
    fprintf(stderr, "Author  : brunql \n");
    fprintf(stderr, "Version : 2.1 \n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s --help  - show this help \n", myName);
    fprintf(stderr, "  %s         - start application \n", myName);
    fprintf(stderr, "  sudo %s    - if can't open device, try this \n", myName);
}

bool openLogFile()
{
    QString filePath = QDir::currentPath();
    QDir dirLogs(filePath);
    if(!dirLogs.exists(filePath  + "/AmbilightLogs/")){
        dirLogs.mkdir("AmbilightLogs");
    }
    filePath +=  "/AmbilightLogs/" + QDateTime::currentDateTime().date().toString("yyyy_MM_dd") + ".log";

    cout << "Writing logs to: " << filePath.toStdString() << endl;

    QFile *logFile = new QFile(filePath);
    if(logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        logStream.setDevice(logFile);
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
    if(argc > 1){
        showHelpMessage(argv[0]);
        return 1;
    }

    if(!openLogFile()){
        cerr << "Log file didn't opened. Exit." << endl;
        return 2;
    }


    Q_INIT_RESOURCE(icons);

    qInstallMsgHandler(messageOutput);
    QApplication app(argc, argv);
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
