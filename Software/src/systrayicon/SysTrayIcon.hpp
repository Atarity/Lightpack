/*
 * SysTrayIcon.hpp
 *
 *  Created on: 11/23/2013
 *     Project: Prismatik 
 *
 *  Copyright (c) 2013 tim
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

#ifndef SYSTRAYICON_HPP
#define SYSTRAYICON_HPP

#include <QObject>
#include "enums.hpp"

class QMenu;
class SysTrayIconPrivate;


class SysTrayIcon : public QObject
{
    Q_OBJECT

    static const QString LightpackDownloadsPageUrl;
public:

    enum Message
    {
        MessageUpdateFirmware,
        MessageAnotherInstance,
        MessageGeneric
    };

    enum Status
    {
        StatusOn,
        StatusOff,
        StatusError,
        StatusLockedByApi,
        StatusLockedByPlugin
    };

    explicit SysTrayIcon(QObject *parent = 0);
    virtual ~SysTrayIcon();
    
    void init();
    bool isVisible() const;
    void showMessage(const QString &title, const QString &text);
    void showMessage(const Message msg);
    void setStatus(const Status status, const QString *arg = NULL);
    void updateProfiles();
    QString toolTip() const;
    void hide();
    void show();

signals:
    void profileSwitched(const QString &profileName);
    void backlightOn();
    void backlightOff();
    void showSettings();
    void toggleSettings();
    void quit();

public slots:
    void retranslateUi();
    void checkUpdate();

private:
    SysTrayIconPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(SysTrayIcon)
    Q_DISABLE_COPY(SysTrayIcon)

};

#endif // SYSTRAYICON_HPP
