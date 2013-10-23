/*
 * main.cpp
 *
 *  Created on: 10/22/2013
 *     Project: %PROJECT% (Use "Lightpack" for hardware/firmware, or "Prismatik" for software)
 *
 *  Copyright (c) 2013 %NICKNAME%
 *
 *  Lightpack is an open-source, USB content-driving ambient lighting
 *  hardware.
 *
 *  Prismatik is a free, open-source software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Prismatik and Lightpack files is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Wizard.hpp"
#include <QApplication>
#include <Settings.hpp>
#include <QFileInfo>
#include <QDir>
#include <iostream>

unsigned g_debugLevel = SettingsScope::Main::DebugLevelDefault;

QString getApplicationDirectoryPath(const char * firstCmdArgument)
{
    using namespace std;

    QFileInfo fileInfo(firstCmdArgument);
    QString appDirPath = fileInfo.absoluteDir().absolutePath();

    QString lightpackMainConfPath = appDirPath + "/main.conf";

    cout << lightpackMainConfPath.toStdString() << endl;

    /// Unportable version
    /// Store logs and settings in home directory of the current user

    cout << "Unportable version" << endl;

#       ifdef Q_WS_WIN
    appDirPath = QDir::homePath() + "/Prismatik";
#       else
    appDirPath = QDir::homePath() + "/.Prismatik";
#       endif

    QDir dir(appDirPath);
    if (dir.exists() == false)
    {
        cout << "mkdir " << appDirPath.toStdString() << endl;
        if (dir.mkdir(appDirPath) == false)
        {
            cerr << "Failed mkdir '" << appDirPath.toStdString() << "' for application generated stuff. Exit." << endl;
            exit(1);
        }
    }

    cout << "Application directory: " << appDirPath.toStdString() << endl;

    return appDirPath;
}

int main(int argc, char *argv[])
{
    using namespace SettingsScope;

    QApplication a(argc, argv);
    Settings settings;
    settings.Initialize(getApplicationDirectoryPath(argv[0]), false);
    Wizard w(&settings);
    w.setWindowFlags(Qt::WindowStaysOnTopHint);
    w.show();

    return a.exec();
}
