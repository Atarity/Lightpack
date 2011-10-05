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


/*! \~english
    \class SerialDeviceEnumerator

    \brief  Class SerialDeviceEnumerator monitors and receives information on all serial devices in the system.

    \section sec0_SerialDeviceEnumerator Appointment and opportunities.

    This class is part of the library QSerialDevice and can be used in conjunction with the class AbstractSerial. \n

    This class is a singleton with a pointer private static object. \n

    This class provides the following features:
    - Get a list of names of all available serial devices in the system.
    - Notify about deleting/adding serial device.
    - Obtain information about any serial device that is in the system.
    .

    The structure of the class is implemented by the ideology \b pimpl. \n

    The principle of class on different operating systems have significant differences:
    - In MS Windows 2K/XP/Vista/7 information about the presence of serial devices is extracted from the Win API,
    and the events by adding/removing serial device is removed from the system registry.
    - In GNU/Linux used to obtain information \b UDEV (http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html).
    .
    
    Class SerialDeviceEnumerator supported on the following operating systems:
    
    <TABLE>
    <TR><TH> Operating System </TH><TH> Support </TH><TH> Note </TH></TR>
    <TR><TD> MS Windows 2K,XP,Vista,7 </TD><TD> Yes </TD><TD> Full </TD></TR>
    <TR><TD> Distributions GNU Linux </TD><TD> Yes </TD><TD> In the presence of Udev </TD></TR>
    <TR><TD> Mac OSX </TD><TD> No </TD><TD> It is not possible to write code and test it. </TD></TR>
    </TABLE>

    This class combines the functions, the such "obsolete" classes as: SerialDeviceWatcher and SerialDeviceInfo.
    Reason for joining was that SerialDeviceWatcher and SerialDeviceInfo use the same type code
    have to duplicate and SerialDeviceWatcher and SerialDeviceInfo, as well as its "complexity". \n

    \note This class is not thread safe and should be used only
    in the context of the main application thread (GUI thread)!

    Differences implementation of SerialDeviceEnumerator SerialDeviceWatcher and SerialDeviceInfo:
    - Contains more than a simple code.
    - It is better to perform the functions detect add/remove new devices
    as code has been improved by removing the read-registry (windows) and analysis of the directory /dev (linux).
    - Faster and optimal update information on the devices at its request.
    .

    \section sec1_SerialDeviceEnumerator A brief description use.
    
    Getting Started with the class should begin with a pointer to the singleton (object SerialDeviceEnumerator). \n
    Example:
    \code
        ...
        SerialDeviceEnumerator *sde = SerialDeviceEnumerator::instance();
        ...
    \endcode

    \note By default, singleton mode control (monitoring) presence (absence) of a serial device is enabled.

    After that, you must associate the signals from SerialDeviceEnumerator slot information processing,
    and for the first time forced to process a list of devices (hereinafter,
    forced to handle a list of devices is not necessary). \n
    Example:
    \code
        void MyAppOrClass::myInitMethodOrConstructor()
        {
            ...
            SerialDeviceEnumerator *enumerator = SerialDeviceEnumerator::instance();
            connect(enumerator, SIGNAL(hasChanged(QStringList)), this, SLOT(mySlotProcDevicesList(QStringList)));
            mySlotProcDevicesList(enumerator->devicesAvailable());
            ...
        }

        void MyAppOrClass::mySlotProcDevicesList(const QStringList &deviesList)
        {
            // Fill ports box (QComboBox), etc.
            ui->portBox->clear();
            ui->portBox->addItems(deviesList);
        }
    \endcode

    \section sec2_SerialDeviceEnumerator Methods tracking and monitoring.

    To enable/disable monitoring for serial devices using the method:
    - void SerialDeviceEnumerator::setEnabled(bool enable) enables or disables monitoring.
    \note Use this method is not desirable!

    For condition monitoring method is used:
    - bool SerialDeviceEnumerator::isEnabled() const returns the current mode of monitoring (tracking active or not).

    \section sec3_SerialDeviceEnumerator Methods get info.

    For a list of names of all available serial devices in the system using the method:
    - QStringList SerialDeviceEnumerator::devicesAvailable() const returns a list of names.

    For information about a specific device, you must first set the name of this device:
    - void SerialDeviceEnumerator::setDeviceName(const QString &name) sets the device name on which we want to get information.
    \note
        - The names in Windows should be "short" (ie without the prefix \\\\.\\, Etc.), for example: COM1 ... COMn.
        - The names of the OS GNU/Linux should be "complete" (ie the full path to the device), for example: /dev/ttyS0 ... /dev/ttySn.
        - Change the name (or reinstall the new name) at any time.
    .

    After the name of the set you can get information, but before you can check the name of your installed devices:
    - QString SerialDeviceEnumerator::name() const returns the name of the device which is currently set for more information.

    For information about the serial device used methods:
    - QString SerialDeviceEnumerator::shortName() const returns the short name of the device.
    - QString SerialDeviceEnumerator::systemPath() returns information about the system path to the serial device.
    - QString SerialDeviceEnumerator::subSystem() returns the name of the subsystem serial device.
    - QString SerialDeviceEnumerator::locationInfo()returns information about the location of the serial device.
    - QString SerialDeviceEnumerator::driver() returns information about the serial device driver.
    - QString SerialDeviceEnumerator::friendlyName() returns a friendly name serial device.
    - QString SerialDeviceEnumerator::description() returns a description of the serial device.
    - QStringList SerialDeviceEnumerator::hardwareID() returns the identifier of the hardware serial devices.
    - QString SerialDeviceEnumerator::vendorID() returns the vendor ID serial device.
    - QString SerialDeviceEnumerator::productID() returns the product ID (chip) serial device.
    - QString SerialDeviceEnumerator::manufacturer() returns the name of the manufacturer's serial device.
    - QString SerialDeviceEnumerator::service() const service returns the serial device.
    - QString SerialDeviceEnumerator::bus() const returns the serial device bus.
    - QString SerialDeviceEnumerator::revision() const returns the serial device revision.
    - bool SerialDeviceEnumerator::isExists() const checks the serial devices in the system.
    - bool SerialDeviceEnumerator::isBusy() const chech serial device is busy.
    .

    \note The methods mentioned above will return incorrect results when disconnected mode monitoring
    so disable monitoring method setEnabled() is not recommended!
    And in general, not recommended to use the setEnabled(), this method is left to future developments.

    \section sec4_SerialDeviceEnumerator Signals.

    SerialDeviceEnumerator class implements the following signals:
    - void SerialDeviceEnumerator::hasChanged(const QStringList &list) automatically emitted when adding/removing the serial device.

    \n
    \n
    \n

    \author Denis Shienkov \n
    Contact:
    - ICQ       : 321789831
    - e-mail    : scapig2@yandex.ru
*/

#include <QtCore/QStringList>

#include "serialdeviceenumerator.h"
#include "serialdeviceenumerator_p.h"

//#define SERIALDEVICEENUMERATOR_DEBUG

#ifdef SERIALDEVICEENUMERATOR_DEBUG
#include <QtCore/QDebug>
#endif


//Private

void SerialDeviceEnumeratorPrivate::setNativeDeviceName(const QString &name)
{
    this->currName = name;
    this->currInfo = this->infoMap.value(name);
}

QString SerialDeviceEnumeratorPrivate::nativeName() const
{
    return this->currName;
}

QString SerialDeviceEnumeratorPrivate::nativeShortName() const
{
    return this->currInfo.shortName;
}

QString SerialDeviceEnumeratorPrivate::nativeSystemPath() const
{
    return this->currInfo.systemPath;
}

QString SerialDeviceEnumeratorPrivate::nativeSubSystem() const
{
    return this->currInfo.subSystem;
}

QString SerialDeviceEnumeratorPrivate::nativeLocationInfo() const
{
    return this->currInfo.locationInfo;
}

QString SerialDeviceEnumeratorPrivate::nativeDriver() const
{
    return this->currInfo.driverName;
}

QString SerialDeviceEnumeratorPrivate::nativeFriendlyName() const
{
    return this->currInfo.friendlyName;
}

QString SerialDeviceEnumeratorPrivate::nativeDescription() const
{
    return this->currInfo.description;
}

QStringList SerialDeviceEnumeratorPrivate::nativeHardwareID() const
{
    return this->currInfo.hardwareID;
}

QString SerialDeviceEnumeratorPrivate::nativeVendorID() const
{
    return this->currInfo.vendorID;
}

QString SerialDeviceEnumeratorPrivate::nativeProductID() const
{
    return this->currInfo.productID;
}

QString SerialDeviceEnumeratorPrivate::nativeManufacturer() const
{
    return this->currInfo.manufacturer;
}

QString SerialDeviceEnumeratorPrivate::nativeService() const
{
    return this->currInfo.service;
}

QString SerialDeviceEnumeratorPrivate::nativeBus() const
{
    return this->currInfo.bus;
}

QString SerialDeviceEnumeratorPrivate::nativeRevision() const
{
    return this->currInfo.revision;
}

bool SerialDeviceEnumeratorPrivate::nativeIsExists() const
{
    return (this->infoMap.keys().contains(this->currName)) ? true : false;
}



/*! \~english
    Static object (Singleton).
*/
SerialDeviceEnumerator *SerialDeviceEnumerator::self = 0;

/*! \~english
    \fn SerialDeviceEnumerator *SerialDeviceEnumerator::instance()
    Create object and returns a pointer to a static object (Singleton).
    \return Pointer to SerialDeviceEnumerator.
*/
SerialDeviceEnumerator *SerialDeviceEnumerator::instance()
{
    if (!self)
        self = new SerialDeviceEnumerator();
    return self;
}

/*! \~english
    \fn SerialDeviceEnumerator::SerialDeviceEnumerator(QObject *parent)
    Default constructor.
*/
SerialDeviceEnumerator::SerialDeviceEnumerator(QObject *parent)
    : QObject(parent), d_ptr(new SerialDeviceEnumeratorPrivate())
{
    Q_D(SerialDeviceEnumerator);
    d->q_ptr = this;

    this->setEnabled(true);
}

/*! \~english
    \fn SerialDeviceEnumerator::~SerialDeviceEnumerator()
    Default destructor.
*/
SerialDeviceEnumerator::~SerialDeviceEnumerator()
{
    delete d_ptr;
}

/*! \~english
    \fn void SerialDeviceEnumerator::setEnabled(bool enable)
    Enables or disables the monitoring regime consistent
    devices depending on the parameter \a enable:
    - If \a enable == true then the mode is monitoring.
    - If \a enable == false then disconnected mode of monitoring.
    .
    \param[in] Enable flag on/off tracking.
*/
void SerialDeviceEnumerator::setEnabled(bool enable)
{
    d_func()->setEnabled(enable);
}

/*! \~english
    \fn bool SerialDeviceEnumerator::isEnabled() const
    Returns the current state of monitoring.
    \return \a True monitoring enabled.
*/
bool SerialDeviceEnumerator::isEnabled() const
{
    return d_func()->isEnabled();
}

/*! \~english
    \fn QStringList SerialDeviceEnumerator::devicesAvailable() const
    Returns a list of all serial devices that are present
    in the system at the moment. In the absence of serial devices
    or error method returns an empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return List of serial devices in a QStringList.
*/
QStringList SerialDeviceEnumerator::devicesAvailable() const
{
    return d_func()->infoMap.keys();
}

/*! \~english
    \fn void SerialDeviceEnumerator::setDeviceName(const QString &name)
    Sets the name \a name serial device
    information about where we want to receive:
    - The MS Windows names should be "short", ie example: COM1 ... COMn.
    - The GNU/Linux names must be "long", ie example: /dev/ttyS0 ... /dev/ttySn.
    .
    \param[in] name Name we are interested in the serial device.
*/
void SerialDeviceEnumerator::setDeviceName(const QString &name)
{
    d_func()->setNativeDeviceName(name);
}

/*! \~english
    \fn QString SerialDeviceEnumerator::name() const
    Returns the name of the serial devices that are currently configured
    and about which we want to get information.
    \return The current name of the serial devices in the form QString.
*/
QString SerialDeviceEnumerator::name() const
{
    return d_func()->nativeName();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::shortName() const
    Returns the short name. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return The short name as a QString.
*/
QString SerialDeviceEnumerator::shortName() const
{
    return d_func()->nativeShortName();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::systemPath() const
    Returns the system path. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Path as a QString.
*/
QString SerialDeviceEnumerator::systemPath() const
{
    return d_func()->nativeSystemPath();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::subSystem() const
    Returns the subsystem. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Subsystem in a QString.
*/
QString SerialDeviceEnumerator::subSystem() const
{
    return d_func()->nativeSubSystem();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::locationInfo() const
    Returns the location. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Location in a QString.
*/
QString SerialDeviceEnumerator::locationInfo() const
{
    return d_func()->nativeLocationInfo();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::driver() const
    Returns the driver. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Driver as QString.
*/
QString SerialDeviceEnumerator::driver() const
{
    return d_func()->nativeDriver();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::friendlyName() const
    Returns the friendly name. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Friendly name as a QString.
*/
QString SerialDeviceEnumerator::friendlyName() const
{
    return d_func()->nativeFriendlyName();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::description() const
    Returns the description. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Description in a QString.
*/
QString SerialDeviceEnumerator::description() const
{
    return d_func()->nativeDescription();
}

/*! \~english
    \fn QStringList SerialDeviceEnumerator::hardwareID() const
    Returns the ID of the hardware. If the information is not found then return the empty list.
    \note When monitoring is turned off the return result is unreliable.
    \return The identifier in the form QStringList.
*/
QStringList SerialDeviceEnumerator::hardwareID() const
{
    return d_func()->nativeHardwareID();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::vendorID() const
    Returns the vendor ID. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return The identifier in a QString.
*/
QString SerialDeviceEnumerator::vendorID() const
{
    return d_func()->nativeVendorID();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::productID() const
    Returns the product ID. If the information is not found then return the empty string.
    \note when disconnected monitoring return result unreliable.
    \return The identifier in a QString.
*/
QString SerialDeviceEnumerator::productID() const
{
    return d_func()->nativeProductID();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::manufacturer() const
    Returns the name of the manufacturer. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Manufacturer as QString.
*/
QString SerialDeviceEnumerator::manufacturer() const
{
    return d_func()->nativeManufacturer();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::service() const
    Returns the name of the service. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Service as a QString.
*/
QString SerialDeviceEnumerator::service() const
{
    return d_func()->nativeService();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::bus() const
    Returns the name of the bus. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Bus as a QString.
*/
QString SerialDeviceEnumerator::bus() const
{
    return d_func()->nativeBus();
}

/*! \~english
    \fn QString SerialDeviceEnumerator::revision() const
    Returns the num of the revision. If the information is not found then return the empty string.
    \note When monitoring is turned off the return result is unreliable.
    \return Revision as a QString.
*/
QString SerialDeviceEnumerator::revision() const
{
    return d_func()->nativeRevision();
}


/*! \~english
    \fn bool SerialDeviceEnumerator::isExists() const
    Checks exists the serial devices in the system at the moment.
    \note When monitoring is turned off the return result is unreliable.
    \return \a True if the serial device exists on the system.
*/
bool SerialDeviceEnumerator::isExists() const
{
    return d_func()->nativeIsExists();
}

/*! \~english
    \fn bool SerialDeviceEnumerator::isBusy() const
    Checks busy the serial devices in the system at the moment.
    \return \a True if the serial device busy on the system.
*/
bool SerialDeviceEnumerator::isBusy() const
{
    return d_func()->nativeIsBusy();
}

/*! \~english
    \fn bool SerialDeviceEnumerator::isEmpty() const
    Check the configuration of the object SerialDeviceInfo on the fact that the object is configured.
    \return \a True if the object is empty, ie does not have the name of the method: setName().
*/

/*! \~english
    \fn bool SerialDeviceEnumerator::isBusy() const
    Checks busy or not the serial device at the moment.
    \return \a True if the device is employed in any process (eg open) or if an error occurred.
*/


/*! \~english
    \fn SerialDeviceEnumerator::hasChanged (const QStringList &list)
    This signal is automatically emitted when adding/removing the serial device.
    The only exception is the first call to setEnabled (true) when the signal is emitted by force!
    In this case the signal is transmitted a list of devices that are present in the system.
    \param[out] list A list of serial devices that are present in the system.
*/



#include "moc_serialdeviceenumerator.cpp"
