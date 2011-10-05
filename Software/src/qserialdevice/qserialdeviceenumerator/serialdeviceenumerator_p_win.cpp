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
#include <QtCore/QVariant>
#include <QtCore/private/qwineventnotifier_p.h>

#include <objbase.h>
#include <initguid.h>
#include <setupapi.h>


#include "serialdeviceenumerator.h"
#include "serialdeviceenumerator_p.h"

//#define SERIALDEVICEENUMERATOR_WIN_DEBUG

#ifdef SERIALDEVICEENUMERATOR_WIN_DEBUG
#include <QtCore/QDebug>
#endif


SerialDeviceEnumeratorPrivate::SerialDeviceEnumeratorPrivate()
        : notifier(0)
{
    this->eHandle = ::CreateEvent(0, false, false, 0);

    if (!this->eHandle) {
#if defined (SERIALDEVICEENUMERATOR_WIN_DEBUG)
        qDebug() << "Windows: SerialDeviceEnumeratorPrivate::setEnabled(..) \n"
                " -> function: ::CreateEvent(..) returned: 0 \n"
                " -> last error: " << ::GetLastError() << ". Error! \n";
#endif
        return;
    }

    QString subKey = "SYSTEM\\CurrentControlSet\\services";
    ::LONG ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                reinterpret_cast<const wchar_t *>(subKey.utf16()),
                                0,
                                KEY_NOTIFY,
                                &this->keyHandle);

    if (ERROR_SUCCESS != ret) {
        this->keyHandle = 0;
#if defined (SERIALDEVICEENUMERATOR_WIN_DEBUG)
        qDebug() << "Windows: SerialDeviceEnumeratorPrivate::setEnabled(..) \n"
                " -> function: ::RegOpenKeyEx(..) returned: " << ret << ". Error! \n";
#endif
        return;
    }
}

SerialDeviceEnumeratorPrivate::~SerialDeviceEnumeratorPrivate()
{
    if (this->notifier) {
        this->notifier->setEnabled(false);
        delete this->notifier;
    }

    if (this->eHandle)
        ::CloseHandle(this->eHandle);

    if (this->keyHandle)
        ::RegCloseKey(this->keyHandle);
}

void SerialDeviceEnumeratorPrivate::setEnabled(bool enable)
{
    Q_Q(SerialDeviceEnumerator);

    if (!this->notifier) {
        if (!this->eHandle)
            return;
        this->notifier = new QWinEventNotifier(this->eHandle, q);
        q->connect(this->notifier, SIGNAL(activated(HANDLE)), q, SLOT(_q_processWatcher()));
    }

    if (!this->isValid())
        return;

    this->notifier->setEnabled(enable);

    if (enable)
        this->_q_processWatcher();
}

bool SerialDeviceEnumeratorPrivate::isEnabled() const
{
    return (this->isValid() && this->notifier->isEnabled());
}

bool SerialDeviceEnumeratorPrivate::nativeIsBusy() const
{
    bool ret = false;
    QString path = this->nativeName();
    if (path.isEmpty())
        return ret;

    path.prepend("\\\\.\\");
    QByteArray nativeFilePath = QByteArray((const char *)path.utf16(), path.size() * 2 + 1);

    ::HANDLE hd = ::CreateFile((const wchar_t*)nativeFilePath.constData(),
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               0,
                               OPEN_EXISTING,
                               0,
                               0);

    if (INVALID_HANDLE_VALUE == hd) {
        ret = true;

        ::LONG err = ::GetLastError();
        switch (err) {
            case ERROR_ACCESS_DENIED:
#if defined (SERIALDEVICEENUMERATOR_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceEnumeratorPrivate::isBusy() \n"
                " -> function: ::GetLastError() returned " << err << " \n"
                " ie Access is denied. \n";
#endif
                break;
            default:
#if defined (SERIALDEVICEENUMERATOR_WIN_DEBUG)
    qDebug() <<"Windows: SerialDeviceEnumeratorPrivate::isBusy() \n"
                " -> function: ::GetLastError() returned " << err << " \n"
                " ie unknown error. \n";
#endif
                ;
        }
    }
    else {
        ::CancelIo(hd);
        if (0 == ::CloseHandle(hd)) {
#if defined (SERIALDEVICEENUMERATOR_WIN_DEBUG)
    qDebug("Windows: SerialDeviceEnumeratorPrivate::isBusy() \n"
            " -> function: ::CloseHandle(hd) returned 0. Error! \n");
#endif
        }
    }
    return ret;
}

static QVariant getDeviceRegistryProperty(::HDEVINFO DeviceInfoSet,
                                          ::PSP_DEVINFO_DATA DeviceInfoData,
                                          ::DWORD property)
{
    ::DWORD dataType = 0;
    ::DWORD dataSize = 0;
    QVariant v;

    ::SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
                                       DeviceInfoData,
                                       property,
                                       &dataType,
                                       0,
                                       0,
                                       &dataSize);

    QByteArray data(dataSize, 0);

    if (::SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
                                           DeviceInfoData,
                                           property,
                                           0,
                                           reinterpret_cast<unsigned char*>(data.data()),
                                           dataSize,
                                           0)) {

        switch (dataType) {

        case REG_EXPAND_SZ:
        case REG_SZ: {
                QString s;
                if (dataSize) {
                    s = QString::fromWCharArray(((const wchar_t *)data.constData()));
                }
                v = QVariant(s);
                break;
            }

        case REG_MULTI_SZ: {
                QStringList l;
                if (dataSize) {
                    int i = 0;
                    for (;;) {
                        QString s = QString::fromWCharArray((const wchar_t *)data.constData() + i);
                        i += s.length() + 1;

                        if (s.isEmpty())
                            break;
                        l.append(s);
                    }
                }
                v = QVariant(l);
                break;
            }

        case REG_NONE:
        case REG_BINARY: {
                QString s;
                if (dataSize) {
                    s = QString::fromWCharArray((const wchar_t *)data.constData(), data.size() / 2);
                }
                v = QVariant(s);
                break;
            }

        case REG_DWORD_BIG_ENDIAN:
        case REG_DWORD: {
                Q_ASSERT(data.size() == sizeof(int));
                int i = 0;
                ::memcpy((void *)(&i), data.constData(), sizeof(int));
                v = i;
                break;
            }

        default:
            v = QVariant();
        }

    }

    return v;
}

//In some Windows (eg XP, and 7) field "Hardware ID" contains the "VID" and "PID" in different registers
//(Upper or Lower case). Therefore, this function depending on the OS selects the appropriate content for
//regular expressions.
//TODO: Perhaps the realization and the idea of a curve and should replace it?
static QString getRegExpContent()
{
    static QString s;
    if (!s.isEmpty())
        return s;

    switch (QSysInfo::WindowsVersion) {
        case QSysInfo::WV_NT:
        case QSysInfo::WV_2000:
        case QSysInfo::WV_XP:
        case QSysInfo::WV_2003:
            s = "Vid_(\\w+)&Pid_(\\w+)"; break;
        case QSysInfo::WV_VISTA:
        case QSysInfo::WV_WINDOWS7:
            s = "VID_(\\w+)&PID_(\\w+)"; break;
        default:;
    }
    return s;
}

//get Vendor ID from HardwareID
static QString getNativeVendorID(const QStringList &hardvareID)
{
    QString s;

    if (hardvareID.isEmpty())
        return s;

    QRegExp re(getRegExpContent());
    s = hardvareID.at(0);
    if (s.contains(re))
        s = re.cap(1);

    return s;
}

//get Product ID from HardwareID
static QString getNativeProductID(const QStringList &hardvareID)
{
    QString s;

    if (hardvareID.isEmpty())
        return s;

    QRegExp re(getRegExpContent());
    s = hardvareID.at(0);
    if (s.contains(re))
        s = re.cap(2);

    return s;
}

//get name of device
static QString getNativeName(::HDEVINFO DeviceInfoSet,
                             ::PSP_DEVINFO_DATA DeviceInfoData) {

    ::HKEY key = ::SetupDiOpenDevRegKey(DeviceInfoSet,
                                        DeviceInfoData,
                                        DICS_FLAG_GLOBAL,
                                        0,
                                        DIREG_DEV,
                                        KEY_READ);

    QString result;

    if (INVALID_HANDLE_VALUE == key)
        return result;

    ::LONG ret = ERROR_SUCCESS;
    ::DWORD i = 0;
    ::DWORD keyType = 0;
    QByteArray buffKeyName(16384, 0);
    QByteArray buffKeyVal(16384, 0);

    for (;;) {
        ::DWORD lenKeyName = buffKeyName.size();
        ::DWORD lenKeyValue = buffKeyVal.size();

        ret = ::RegEnumValue(key,
                             i++,
                             reinterpret_cast<wchar_t *>(buffKeyName.data()),
                             &lenKeyName,
                             0,
                             &keyType,
                             reinterpret_cast<unsigned char*>(buffKeyVal.data()),
                             &lenKeyValue);

        if (ERROR_SUCCESS == ret) {
            if (REG_SZ == keyType) {

                QString itemName = QString::fromUtf16(reinterpret_cast<ushort*>(buffKeyName.data()), lenKeyName);
                QString itemValue = QString::fromUtf16(((const ushort*)buffKeyVal.constData()));

                if (itemName.contains("PortName")) {
                    result = itemValue;
                    break;
                }
            }
        }
        else
            break;
    }

    ::RegCloseKey(key);

    return result;
}

//get device driver name
static QString getNativeDriver(const QString &service)
{
    QString result;
    QString subKey("SYSTEM\\CurrentControlSet\\services\\");

    subKey.append(service);

    ::HKEY key;
    ::LONG res = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                reinterpret_cast<const wchar_t *>(subKey.utf16()),
                                0,
                                KEY_QUERY_VALUE,
                                &key);

    if (ERROR_SUCCESS == res) {

        DWORD dataType = 0;
        DWORD dataSize = 0;

        subKey = "ImagePath";
        res = ::RegQueryValueEx(key,
                                reinterpret_cast<const wchar_t *>(subKey.utf16()),
                                0,
                                &dataType,
                                0,
                                &dataSize);

        if (ERROR_SUCCESS == res) {

            QByteArray data(dataSize, 0);
            res = ::RegQueryValueEx(key,
                                    reinterpret_cast<const wchar_t *>(subKey.utf16()),
                                    0,
                                    0,
                                    reinterpret_cast<unsigned char*>(data.data()),
                                    &dataSize);

            if (ERROR_SUCCESS == res) {

                switch (dataType) {
                case REG_EXPAND_SZ:
                case REG_SZ: {
                        if (dataSize) {
                            result = QString::fromWCharArray(((const wchar_t *)data.constData()));

                        }
                        break;
                    }
                default:;
                }
            }
        }
        ::RegCloseKey(key);
    }
    return result;
}

static const ::GUID guidArray[] =
{
    /* Windows Ports Class GUID */
    { 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },
    /* Virtual Ports Class GUIG (i.e. com0com, nmea and etc) */
    { 0xDF799E12, 0x3C56, 0x421B, { 0xB2, 0x98, 0xB6, 0xD3, 0x64, 0x2B, 0xC8, 0x78 } },
    /* Windows Modems Class GUID */
    { 0x4D36E96D, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } }
};

SerialInfoMap SerialDeviceEnumeratorPrivate::updateInfo() const
{
   SerialInfoMap info;

   static int guidCount = sizeof(guidArray)/sizeof(::GUID);
   for (int i = 0; i < guidCount; ++i) {

       ::HDEVINFO DeviceInfoSet = ::SetupDiGetClassDevs(&guidArray[i],
                                                        0,
                                                        0,
                                                        DIGCF_PRESENT);

       if (INVALID_HANDLE_VALUE == DeviceInfoSet) {
#if defined (SERIALDEVICEENUMERATOR_WIN_DEBUG)
           qDebug() << "Windows: SerialDeviceEnumeratorPrivate::updateInfo() \n"
                   " -> ::SetupDiGetClassDevs() returned INVALID_HANDLE_VALUE, \n"
                   " -> last error: " << ::GetLastError() << ". Error! \n";
#endif
           return info;
       }

       ::SP_DEVINFO_DATA DeviceInfoData = {0};
       //::memset(&DeviceInfoData, 0, sizeof(::SP_DEVINFO_DATA));
       DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

       ::DWORD DeviceIndex = 0;
       while (::SetupDiEnumDeviceInfo(DeviceInfoSet,
                                      DeviceIndex++,
                                      &DeviceInfoData)) {

           SerialInfo si;
           QVariant v;

           //get device name
           v = getNativeName(DeviceInfoSet, &DeviceInfoData);
           QString s = v.toString();

           if ((!s.isEmpty()) && (!s.contains("LPT"))) {
               //bus
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_ENUMERATOR_NAME);
               si.bus = v.toString();
               //description
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_DEVICEDESC);
               si.description = v.toString();
               //friendly name
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME);
               si.friendlyName = v.toString();
               //hardware ID
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID);
               si.hardwareID = v.toStringList();
               //location info
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_LOCATION_INFORMATION);
               si.locationInfo = v.toString();
               //manufacturer
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_MFG);
               si.manufacturer = v.toString();
               //sub system
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_CLASS);
               si.subSystem = v.toString();
               //service
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_SERVICE);
               si.service = v.toString();
               //driver
               v = getNativeDriver(si.service);
               si.driverName = v.toString();
               //system path
               v = getDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME);
               si.systemPath = v.toString();
               //product ID
               si.productID = getNativeProductID(si.hardwareID);
               //vendor ID
               si.vendorID = getNativeVendorID(si.hardwareID);
               //short name
               si.shortName = s;

               //add to map
               info[s] = si;
           }
       }

       ::SetupDiDestroyDeviceInfoList(DeviceInfoSet);
   }
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

    ::LONG ret = ::RegNotifyChangeKeyValue(this->keyHandle,
                                           true,
                                           REG_NOTIFY_CHANGE_LAST_SET,
                                           this->eHandle,
                                           true);

    if (ERROR_SUCCESS != ret) {
#if defined (SERIALDEVICEENUMERATOR_WIN_DEBUG)
        qDebug() << "Windows: SerialDeviceEnumeratorPrivate::_q_processWatcher() \n"
                " -> function: ::RegNotifyChangeKeyValue(..) returned: " << ret << ". Error! \n";
#endif
    }
}

bool SerialDeviceEnumeratorPrivate::isValid() const
{
    return (this->eHandle && this->notifier && this->keyHandle);
}
