/*
 * SysTrayIcon_qt_p.hpp
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

#ifndef SYSTRAYICON_QT_P_HPP
#define SYSTRAYICON_QT_P_HPP
#include "SysTrayIcon.hpp"
#include "QSystemTrayIcon"
#include "Settings.hpp"
#include "QAction"
#include "QDesktopServices"
#include "QCache"
#include "QMenu"
#include "QUrl"

class SysTrayIconPrivate : QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(SysTrayIcon)

public:
    SysTrayIconPrivate(SysTrayIcon * q)
        : _qsystray(new QSystemTrayIcon())
        , q_ptr(q)
    {
        connect(_qsystray, SIGNAL(activated(QSystemTrayIcon::ActivationReason))
                , this, SLOT(onTrayIcon_Activated(QSystemTrayIcon::ActivationReason)));
        connect(_qsystray, SIGNAL(messageClicked())
                , this, SLOT(onTrayIcon_MessageClicked()));
        QMenu *trayIconMenu = new QMenu();

        createActions();

        fillProfilesFromSettings();

        trayIconMenu->addAction(_switchOnBacklightAction);
        trayIconMenu->addAction(_switchOffBacklightAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addMenu(_profilesMenu);
        trayIconMenu->addAction(_settingsAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(_quitAction);

        _pixmapCache.insert("lock16", new QPixmap(QPixmap(":/icons/lock.png").scaledToWidth(16, Qt::SmoothTransformation)) );
        _pixmapCache.insert("on16", new QPixmap(QPixmap(":/icons/on.png").scaledToWidth(16, Qt::SmoothTransformation)) );
        _pixmapCache.insert("off16", new QPixmap(QPixmap(":/icons/off.png").scaledToWidth(16, Qt::SmoothTransformation)) );
        _pixmapCache.insert("error16", new QPixmap(QPixmap(":/icons/error.png").scaledToWidth(16, Qt::SmoothTransformation)) );

        _qsystray = new QSystemTrayIcon();
        _qsystray->setContextMenu(trayIconMenu);
        setStatus(SysTrayIcon::StatusOn);
        _qsystray->show();
    }

    virtual ~SysTrayIconPrivate()
    {
        delete _qsystray->contextMenu();
        delete _profilesMenu;
        delete _qsystray;
    }

    bool isVisible() const
    {
        return _qsystray->isVisible();
    }

    void showMessage(const QString &title, const QString &msg)
    {
        _trayMessage = SysTrayIcon::MessageGeneric;
        _qsystray->showMessage(title, msg);
    }

    void showMessage(const SysTrayIcon::Message msg)
    {
        switch (msg)
        {
        case SysTrayIcon::MessageAnotherInstance:
            _qsystray->showMessage(tr("Prismatik"), tr("Application already running"));
            break;
        case SysTrayIcon::MessageUpdateFirmware:
            _qsystray->showMessage(tr("Lightpack firmware update"), tr("Click on this message to open lightpack downloads page"));
            break;
        default:
            _trayMessage = SysTrayIcon::MessageGeneric;
            return;
        }
        _trayMessage = msg;
    }

    void show()
    {
        _qsystray->show();
    }

    void hide()
    {
        _qsystray->hide();
    }

    const QString & toolTip() const
    {
        return _qsystray->toolTip();
    }

    void retranslateUi()
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO;
        _switchOnBacklightAction->setText(tr("&Turn on"));
        _switchOffBacklightAction->setText(tr("&Turn off"));
        _settingsAction->setText(tr("&Settings"));
        _quitAction->setText(tr("&Quit"));
        _profilesMenu->setTitle(tr("&Profiles"));

        fillProfilesFromSettings();

        switch (_status)
        {
        case SysTrayIcon::StatusOn:
            _qsystray->setToolTip(tr("Enabled profile: %1").arg(SettingsScope::Settings::getCurrentProfileName()));
            break;
        case SysTrayIcon::StatusOff:
            _qsystray->setToolTip(tr("Disabled"));
            break;
        case SysTrayIcon::StatusError:
            _qsystray->setToolTip(tr("Error with connection device, verbose in logs"));
            break;
        default:
            qWarning() << Q_FUNC_INFO << "_status contains crap =" << _status;
            break;
        }
    }

    void setStatus(const SysTrayIcon::Status status, const QString *arg = NULL)
    {
        switch (status)
        {
        case SysTrayIcon::StatusOn:
            _switchOnBacklightAction->setEnabled(false);
            _switchOffBacklightAction->setEnabled(true);
            _qsystray->setIcon(QIcon(*_pixmapCache["on16"]));
            _qsystray->setToolTip(tr("Enabled profile: %1").arg(SettingsScope::Settings::getCurrentProfileName()));
            break;

        case SysTrayIcon::StatusLockedByApi:
            _switchOnBacklightAction->setEnabled(false);
            _switchOffBacklightAction->setEnabled(true);
            _qsystray->setIcon(QIcon(*_pixmapCache["lock16"]));
            _qsystray->setToolTip(tr("Device locked via API"));
            break;
        case SysTrayIcon::StatusLockedByPlugin:
            _qsystray->setIcon(QIcon(*_pixmapCache["lock16"]));
            _qsystray->setToolTip(tr("Device locked via Plugin")+" ("+(*arg)+")");
            break;

        case SysTrayIcon::StatusOff:
            _switchOnBacklightAction->setEnabled(true);
            _switchOffBacklightAction->setEnabled(false);
            _qsystray->setIcon(QIcon(*_pixmapCache["off16"]));
            _qsystray->setToolTip(tr("Disabled"));
            break;

        case SysTrayIcon::StatusError:
            _switchOnBacklightAction->setEnabled(false);
            _switchOffBacklightAction->setEnabled(true);
            _qsystray->setIcon(QIcon(*_pixmapCache["error16"]));
            _qsystray->setToolTip(tr("Error with connection device, verbose in logs"));
            break;
        default:
            qWarning() << Q_FUNC_INFO << "status = " << status;
            return;
            break;
        }
        _status = status;
    }

    void updateProfiles()
    {
        fillProfilesFromSettings();
    }

private slots:

    void onProfileAction_Triggered()
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO;
        Q_Q(SysTrayIcon);
        QString profileName = static_cast<QAction*>(sender())->text();
        q->profileSwitched(profileName);
    }

    void onTrayIcon_MessageClicked()
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << _trayMessage;

        switch(_trayMessage)
        {
        case SysTrayIcon::MessageUpdateFirmware:
            if (SettingsScope::Settings::getConnectedDevice() == SupportedDevices::DeviceTypeLightpack)
            {
                // Open lightpack downloads page
                QDesktopServices::openUrl(QUrl(SysTrayIcon::LightpackDownloadsPageUrl, QUrl::TolerantMode));
            }
            break;
        default:
            break;
        }
    }

    void onTrayIcon_Activated(QSystemTrayIcon::ActivationReason reason)
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO;
        Q_Q(SysTrayIcon);

        switch (reason)
        {
        case QSystemTrayIcon::DoubleClick:
            switch (_status)
            {
            case SysTrayIcon::StatusOff:
                setStatus(SysTrayIcon::StatusOn);
                q->backlightOn();
                break;
            case SysTrayIcon::StatusOn:
            case SysTrayIcon::StatusLockedByApi:
            case SysTrayIcon::StatusLockedByPlugin:
            case SysTrayIcon::StatusError:
                setStatus(SysTrayIcon::StatusOff);
                q->backlightOff();
                break;
            default:
                qWarning() << Q_FUNC_INFO << "_status = " << _status;
                break;
            }
            break;

        case QSystemTrayIcon::MiddleClick:
            if (this->isVisible())
            {
                q->hideSettings();
            } else {
                q->showSettings();
            }
            break;

    #   ifdef Q_OS_WIN
        case QSystemTrayIcon::Context:
            // Hide the tray after losing focus
            //
            // In Linux (Ubuntu 10.04) this code grab keyboard input and
            // not give it back to the text editor after close application with
            // "Quit" button in the tray menu
            _trayIconMenu->activateWindow();
            break;
    #   endif


        default:
            ;
        }
    }

private:

    void createActions()
    {
        Q_Q(SysTrayIcon);
        DEBUG_LOW_LEVEL << Q_FUNC_INFO;

        _switchOnBacklightAction = new QAction(QIcon(":/icons/on.png"), tr("&Turn on"), this);
        _switchOnBacklightAction->setIconVisibleInMenu(true);
        connect(_switchOnBacklightAction, SIGNAL(triggered()), q, SIGNAL(backlightOn()));

        _switchOffBacklightAction = new QAction(QIcon(":/icons/off.png"), tr("&Turn off"), this);
        _switchOffBacklightAction->setIconVisibleInMenu(true);
        connect(_switchOffBacklightAction, SIGNAL(triggered()), q, SIGNAL(backlightOff()));


        _profilesMenu = new QMenu(tr("&Profiles"));
        _profilesMenu->setIcon(QIcon(":/icons/profiles.png"));
        _profilesMenu->clear();

        _settingsAction = new QAction(QIcon(":/icons/settings.png"), tr("&Settings"), this);
        _settingsAction->setIconVisibleInMenu(true);
        connect(_settingsAction, SIGNAL(triggered()), q, SIGNAL(showSettings()));

        _quitAction = new QAction(tr("&Quit"), this);
        connect(_quitAction, SIGNAL(triggered()), q, SIGNAL(quit()));
    }

    void fillProfilesFromSettings()
    {
        using namespace SettingsScope;
        QAction *profileAction;

        _profilesMenu->clear();

        QStringList profiles = Settings::findAllProfiles();

        for(int i=0; i < profiles.count(); i++) {
            profileAction = new QAction(profiles[i], this);
            profileAction->setCheckable(true);
            if( profiles[i].compare(Settings::getCurrentProfileName()) == 0 ) {
                profileAction->setChecked(true);
            }
            _profilesMenu->addAction(profileAction);
            connect(profileAction, SIGNAL(triggered()), this, SLOT(onProfileAction_Triggered()));
        }
    }

private:
    QSystemTrayIcon * _qsystray;
    QAction * _switchOnBacklightAction;
    QAction * _switchOffBacklightAction;
    QAction * _settingsAction;
    QAction * _quitAction;
    QMenu * _profilesMenu;
    SysTrayIcon::Message _trayMessage;
    SysTrayIcon::Status _status;

    QCache<QString, QPixmap> _pixmapCache;

    SysTrayIcon * const q_ptr;
};

#endif // SYSTRAYICON_QT_P_HPP
