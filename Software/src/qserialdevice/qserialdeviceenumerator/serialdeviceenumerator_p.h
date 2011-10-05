/*
* This file is part of QSerialDevice, an open-source cross-platform library
* Copyright (C) 2009  Denis Shienkov
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact Denis Shienkov:
*          e-mail: <scapig2@yandex.ru>
*             ICQ: 321789831
*/

#ifndef SERIALDEVICEENUMERATOR_P_H
#define SERIALDEVICEENUMERATOR_P_H

#include <QtCore/QMap>

#include "serialdeviceenumerator.h"

/*
  Inner class for storing information received on a one serial device.
  Fields of class are some options.
*/
class SerialInfo
{
public:
    QString shortName;
    QString systemPath;
    QString subSystem;
    QString locationInfo;
    QString driverName;
    QString friendlyName;
    QString description;
    QStringList hardwareID;
    QString vendorID;
    QString productID;
    QString manufacturer;
    QString service;
    QString bus;
    QString revision;
    //
    inline bool operator!=(const SerialInfo &c) const
    {
        return (!((this->bus == c.bus)
                && (this->description == c.description)
                && (this->driverName == c.driverName)
                && (this->friendlyName == c.friendlyName)
                && (this->hardwareID == c.hardwareID)
                && (this->locationInfo == c.locationInfo)
                && (this->manufacturer == c.manufacturer)
                && (this->productID == c.productID)
                && (this->revision == c.revision)
                && (this->service == c.service)
                && (this->shortName == c.shortName)
                && (this->subSystem == c.subSystem)
                && (this->systemPath == c.systemPath)
                && (this->vendorID == c.vendorID)));
    }
};

/*
  Inner class for storing information received from all found the serial devices.
  The class inherits from QMap, where as a key (QString), use the name of the
  serial devices as well as the value of the class SerialInfo.
*/
class SerialInfoMap : public QMap<QString, SerialInfo>
{
public:
    inline bool operator!=(const SerialInfoMap &m) const
    {
        int size = this->size();
        if ((m.size() != size)
            || (this->keys() != m.keys())) {
            return true;
        }

        QList<SerialInfo> l1 = this->values();
        QList<SerialInfo> l2 = m.values();

        while (size--) {
            if (l1.at(size) != l2.at(size))
                return true;
        }
        return false;
    }
};

#if defined (Q_OS_WIN)
  #include <qt_windows.h>
  class QWinEventNotifier;
#elif defined (Q_OS_MAC)
  #include <CoreFoundation/CoreFoundation.h>
  #include <IOKit/IOKitLib.h>
#elif defined (Q_OS_UNIX)
  struct udev;
  struct udev_monitor;
  class QSocketNotifier;
#endif

class SerialDeviceEnumeratorPrivate
{
    Q_DECLARE_PUBLIC(SerialDeviceEnumerator)
public:
            SerialDeviceEnumeratorPrivate();
    virtual ~SerialDeviceEnumeratorPrivate();

    void setEnabled(bool enable);
    bool isEnabled() const;

    void setNativeDeviceName(const QString &name);
    QString nativeName() const;
    QString nativeShortName() const;
    QString nativeSystemPath() const;
    QString nativeSubSystem() const;
    QString nativeLocationInfo() const;
    QString nativeDriver() const;
    QString nativeFriendlyName() const;
    QString nativeDescription() const;
    QStringList nativeHardwareID() const;
    QString nativeVendorID() const;
    QString nativeProductID() const;
    QString nativeManufacturer() const;
    QString nativeService() const;
    //
    QString nativeBus() const;
    QString nativeRevision() const;

    bool nativeIsExists() const;
    bool nativeIsBusy() const;

    SerialDeviceEnumerator * q_ptr;

#if defined (Q_OS_MAC)
    void notifierHandler();
#endif

private:
    SerialInfoMap infoMap; /* It stores information about all found devices with serial interface. */
    SerialInfoMap updateInfo() const;
    QString currName; /* It contains the current name of the serial device about which receives the information.
                        (The name set the setNativeDeviceName()). */
    SerialInfo currInfo; /* It contains the current info of the serial device about which receives the information.
                        (The name set the setNativeDeviceName()). */
#if defined (Q_OS_WIN)
    ::HANDLE eHandle;
    ::HKEY keyHandle;
    QWinEventNotifier *notifier;
#elif defined (Q_OS_MAC)
    ////
    CFMutableDictionaryRef classesToMatch;
    IONotificationPortRef notifier;
    CFRunLoopSourceRef loop;
    bool enabled;
#elif defined (Q_OS_UNIX)
    struct udev *udev;
    int udev_socket;
    struct udev_monitor *udev_monitor;
    QSocketNotifier *notifier;
    QMap<QString, QString> eqBusDrvMap; /* It contains the line name bus device and its driver.
                                            Completed manually by the programmer. */
    QStringList devNamesMask; /* Contains a list of masks device names for which there is filtration.
                                 Completed manually by the programmer. */
#endif
    //
    void _q_processWatcher();
    bool isValid() const;
};

#endif // SERIALDEVICEENUMERATOR_P_H
