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


#include <QtCore/QStringList>
#include <QtCore/QSocketNotifier>

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

extern "C"
{
    #include <libudev.h>
}

#include "serialdeviceenumerator.h"
#include "serialdeviceenumerator_p.h"
#include "../unix/ttylocker.h"

//#define SERIALDEVICEENUMERATOR_UNIX_DEBUG

#ifdef SERIALDEVICEENUMERATOR_UNIX_DEBUG
#include <QtCore/QDebug>
#endif


SerialDeviceEnumeratorPrivate::SerialDeviceEnumeratorPrivate()
    : notifier(0)
{
    this->udev = ::udev_new();
    if (!this->udev) {
#if defined (SERIALDEVICEENUMERATOR_UNIX_DEBUG)
        qDebug() << "Unix: SerialDeviceEnumeratorPrivate() \n"
                " -> function: ::udev_new() returned: 0 \n";
#endif
        return;
    }

    this->udev_monitor = ::udev_monitor_new_from_netlink(this->udev, "udev");
    if (!this->udev_monitor) {
#if defined (SERIALDEVICEENUMERATOR_UNIX_DEBUG)
        qDebug() << "Unix: SerialDeviceEnumeratorPrivate() \n"
                " -> function: ::udev_monitor_new_fronetlink() returned: 0 \n";
#endif
        return;
    }

    ::udev_monitor_filter_add_match_subsystem_devtype(this->udev_monitor, "tty", 0);
    ::udev_monitor_enable_receiving(this->udev_monitor);
    this->udev_socket = ::udev_monitor_get_fd(this->udev_monitor);

    //Fill the map equivalence name Bus and driver ID.
    this->eqBusDrvMap["usb"] = QString("ID_USB_DRIVER");
    //...
    // Fill here for other associates Bus <-> Driver.
    //
    //..............................................

    //Fill mask devices names list specific by OS.
    //Here you add new name unknow device!
#if defined (Q_OS_LINUX)
    this->devNamesMask
            << "ttyS"       /* standart UART 8250 and etc. */
            << "ttyUSB"     /* usb/serial converters PL2303 and etc. */
            << "ttyACM"     /* CDC_ACM converters (i.e. Mobile Phones). */
            << "ttyMI"      /* MOXA pci/serial converters. */
            << "rfcomm";    /* Bluetooth serial device. */
            //This add other devices mask.
#endif

}

SerialDeviceEnumeratorPrivate::~SerialDeviceEnumeratorPrivate()
{
    if (this->notifier)
        this->notifier->setEnabled(false);

    if (-1 != this->udev_socket) {
        //qt_safe_close(this->udev_socket);
        ::close(this->udev_socket);
        this->udev_socket = -1;
    }

    if (this->udev_monitor)
        ::udev_monitor_unref(this->udev_monitor);

    if (this->udev)
        ::udev_unref(this->udev);
}

void SerialDeviceEnumeratorPrivate::setEnabled(bool enable)
{
    Q_Q(SerialDeviceEnumerator);

    if (!this->notifier) {
        if (-1 == this->udev_socket)
            return;
        this->notifier = new QSocketNotifier(this->udev_socket, QSocketNotifier::Read, q);
        q->connect(this->notifier, SIGNAL(activated(int)), q, SLOT(_q_processWatcher()));
    }

    if (!this->isValid())
        return;

    this->notifier->setEnabled(enable);

    if (enable)
        this->_q_processWatcher();
}

bool SerialDeviceEnumeratorPrivate::isEnabled() const
{
    return (this->isValid() && this->notifier && this->notifier->isEnabled());
}

bool SerialDeviceEnumeratorPrivate::nativeIsBusy() const
{
    bool ret = false;
    QString path = this->nativeName();
    if (path.isEmpty())
        return ret;

    TTYLocker locker;
    locker.setDeviceName(path);

    bool byCurrPid = false;
    ret = locker.locked(&byCurrPid);

    return ret;
}

SerialInfoMap SerialDeviceEnumeratorPrivate::updateInfo() const
{
    SerialInfoMap info;

    struct udev_enumerate *enumerate = ::udev_enumerate_new(this->udev);
    if (!enumerate) {
#if defined (SERIALDEVICEENUMERATOR_UNIX_DEBUG)
        qDebug() << "Unix: SerialDeviceEnumeratorPrivate::updateInfo() \n"
                " -> function: ::udev_enumerate_new() returned: 0 \n";
#endif
        return info;
    }

    struct udev_list_entry *devices, *dev_list_entry;

    ::udev_enumerate_add_match_subsystem(enumerate, "tty");
    ::udev_enumerate_scan_devices(enumerate);

    devices = ::udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {

        const char *syspath = ::udev_list_entry_get_name(dev_list_entry);
        struct udev_device *udev_device = ::udev_device_new_from_syspath(this->udev, syspath);

        if (udev_device) {

            SerialInfo si;
            //get device name
            QString s(::udev_device_get_devnode(udev_device));

            foreach (QString mask, this->devNamesMask) {

                if (s.contains(mask)) {

                    //description
                    si.description = QString(::udev_device_get_property_value(udev_device, "ID_MODEL_FROM_DATABASE"));
                    //revision
                    si.revision = QString(::udev_device_get_property_value(udev_device, "ID_REVISION"));
                    //bus
                    si.bus = QString(::udev_device_get_property_value(udev_device, "ID_BUS"));
                    //driver
                    si.driverName =
                            QString(::udev_device_get_property_value(udev_device,
                                                                     this->eqBusDrvMap.value(si.bus).
                                                                     toLocal8Bit().constData()));
                    //hardware ID
                    si.hardwareID = QStringList();
                    //location info
                    si.locationInfo = QString(::udev_device_get_property_value(udev_device, "ID_MODEL_ENC"))
                                        .replace("\\x20", QString(" "));
                    //manufacturer
                    si.manufacturer = QString(::udev_device_get_property_value(udev_device, "ID_VENDOR_FROM_DATABASE"));
                    //sub system
                    si.subSystem = QString(::udev_device_get_property_value(udev_device, "SUBSYSTEM"));
                    //service
                    si.service = QString();
                    //system path
                    si.systemPath = QString(::udev_device_get_syspath(udev_device));
                    //product ID
                    si.productID = QString(::udev_device_get_property_value(udev_device, "ID_MODEL_ID"));
                    //vendor ID
                    si.vendorID = QString(::udev_device_get_property_value(udev_device, "ID_VENDOR_ID"));
                    //short name
                    si.shortName = QString(::udev_device_get_property_value(udev_device, "DEVNAME"));
                    //friendly name
                    si.friendlyName = si.description + " (" + si.shortName +")";

                    //add to map
                    info[s] = si;
                }
            }
            ::udev_device_unref(udev_device);
        }
    }

    ::udev_enumerate_unref(enumerate);
    return info;
}

void SerialDeviceEnumeratorPrivate::_q_processWatcher()
{
    Q_Q(SerialDeviceEnumerator);

    if (!this->isValid())
        return;

    SerialInfoMap info = this->updateInfo();

    if (info != this->infoMap) {
        this->infoMap = info;
        emit q->hasChanged(info.keys());
    }
}

bool SerialDeviceEnumeratorPrivate::isValid() const
{
    return (this->udev && this->udev_monitor && (-1 != this->udev_socket));
}
