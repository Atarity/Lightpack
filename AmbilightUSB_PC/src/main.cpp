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

#include "mainwindow.h"

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

void messageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}


int main(int argc, char **argv)
{
    if(argc > 1){
        showHelpMessage(argv[0]);
        return 0;
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
