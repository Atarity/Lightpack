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

/*
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
*/
#include <sys/param.h>


#include <IOKit/serial/IOSerialKeys.h>
#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)
#include <IOKit/serial/ioss.h>
#endif
#include <IOKit/IOBSD.h>

//#include <CoreFoundation/CFNumber.h>
//#include <IOKit/usb/IOUSBLib.h>

#include "serialdeviceenumerator.h"
#include "serialdeviceenumerator_p.h"
#include "ttylocker.h"

#define SERIALDEVICEENUMERATOR_MAC_DEBUG

#ifdef SERIALDEVICEENUMERATOR_MAC_DEBUG
#include <QtCore/QDebug>
#endif


SerialDeviceEnumeratorPrivate::SerialDeviceEnumeratorPrivate()
    : classesToMatch(0), notifier(0), loop(0), enabled(false)
{
    this->classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
    if (this->classesToMatch) {
        CFDictionaryAddValue(this->classesToMatch,
                             CFSTR(kIOSerialBSDTypeKey),
                             CFSTR(kIOSerialBSDAllTypes));
    }
}

SerialDeviceEnumeratorPrivate::~SerialDeviceEnumeratorPrivate()
{
    // Here delete notifier, loop, classesToMatch.
    //...
}

static void notifier_handler(void *refCon, io_iterator_t iterator)
{
    qDebug() << "Test: notifier_handler()";
/*
    if (refCon) {
        SerialDeviceEnumeratorPrivate *p = reinterpret_cast<SerialDeviceEnumeratorPrivate *> (refCon);
        if (p) {
            p->notifierHandler();
        }
    }
*/
}

void SerialDeviceEnumeratorPrivate::setEnabled(bool enable)
{
    //Q_Q(SerialDeviceEnumerator);
    kern_return_t kernResult = KERN_FAILURE;
    if (enable && (!this->enabled)) {

        if (!this->notifier) {
            mach_port_t masterPort;
            kernResult = IOMasterPort(MACH_PORT_NULL, &masterPort);

            if (KERN_SUCCESS == kernResult)
                this->notifier = IONotificationPortCreate(masterPort);

            if (!this->notifier)
                return;
        }

        if (!this->loop) {
            this->loop = IONotificationPortGetRunLoopSource(this->notifier);

            if (!this->loop)
                return;

            CFRunLoopAddSource(CFRunLoopGetCurrent(), this->loop, kCFRunLoopDefaultMode);
        }

        if (this->classesToMatch) {

            this->classesToMatch = (CFMutableDictionaryRef) CFRetain(this->classesToMatch);
            //CFMutableDictionaryRef matchingDictionary = (CFMutableDictionaryRef) CFRetain(this->classesToMatch);

            //if (!matchingDictionary)
            //    return;

            io_iterator_t iterator;
            kernResult = IOServiceAddMatchingNotification(this->notifier, kIOFirstMatchNotification,
                                                         this->classesToMatch, notifier_handler, this, &iterator);
            if (KERN_SUCCESS == kernResult) {

                qDebug() << "Test: Enabled kIOFirstMatchNotification";

                this->enabled = true;

                notifier_handler(this, iterator);
                qDebug() << "Test: End enabled";
            }
            /*
            kernResult = IOServiceAddMatchingNotification(this->notifier, kIOTerminatedNotification,
                                                         this->classesToMatch, notifier_handler, this, &iterator);
            if (KERN_SUCCESS == kernResult) {

                qDebug() << "Enabled kIOTerminatedNotification";

                this->enabled = true;

                notifier_handler(this, iterator);
            }
            */
        }
    }
    else {
        if (!enable && this->enabled) {

            if (this->notifier ) {

                //FIXME: here make disable notification.

            }
        }
    }
}

bool SerialDeviceEnumeratorPrivate::isEnabled() const
{
    return this->enabled;
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

    if (this->classesToMatch) {

        io_iterator_t serialPortIterator = 0;
        kern_return_t kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                                                classesToMatch,
                                                                &serialPortIterator);
        if (KERN_SUCCESS == kernResult) {

            io_object_t serialService;
            while ((serialService = IOIteratorNext(serialPortIterator))) {

                SerialInfo si;
                //Device name
                CFTypeRef nameRef = IORegistryEntryCreateCFProperty(serialService,
                                                                      CFSTR(kIOCalloutDeviceKey),
                                                                      kCFAllocatorDefault,
                                                                      0);
                bool result = false;
                QByteArray bsdPath(MAXPATHLEN, 0);
                if (nameRef) {
                    result = CFStringGetCString(CFStringRef(nameRef), bsdPath.data(), bsdPath.size(),
                                                kCFStringEncodingUTF8);
                    CFRelease(nameRef);
                }

                // If device name (path) found.
                if (result) {

                    io_registry_entry_t parent;
                    kernResult = IORegistryEntryGetParentEntry(serialService, kIOServicePlane, &parent);

                    CFTypeRef descriptionRef = 0;
                    CFTypeRef manufacturerRef = 0;
                    CFTypeRef busRef = 0;
                    CFTypeRef subsysRef = 0;
                    CFTypeRef driverRef = 0;
                    CFTypeRef serviceRef = 0;
                    CFTypeRef vidRef = 0;
                    CFTypeRef pidRef = 0;

                    //
                    while ((KERN_SUCCESS == kernResult)) {

                        // Description ref
                        if (!descriptionRef)
                            descriptionRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                             CFSTR("USB Product Name"),
                                                                             kCFAllocatorDefault, 0);
                        // Manufacturer ref
                        if (!manufacturerRef)
                            manufacturerRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                              CFSTR("USB Vendor Name"),
                                                                              kCFAllocatorDefault, 0);
                        // Bus ref
                        if (!busRef)
                            busRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                       CFSTR("IOName"),
                                                                       kCFAllocatorDefault, 0);
                        // Subsys ref
                        if (!subsysRef)
                            subsysRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                       CFSTR("IOTTYBaseName"),
                                                                       kCFAllocatorDefault, 0);
                        // Driver ref
                        if (!driverRef)
                            driverRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                       CFSTR("CFBundleIdentifier"),
                                                                       kCFAllocatorDefault, 0);
                        // Service ref
                        if (!serviceRef)
                            serviceRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                       CFSTR("IOProviderClass"),
                                                                       kCFAllocatorDefault, 0);
                        // Vendor ID ref
                        if (!vidRef)
                            vidRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                     CFSTR("idVendor"),
                                                                     kCFAllocatorDefault, 0);
                        // Product ID ref
                        if (!pidRef)
                            pidRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                     CFSTR("idProduct"),
                                                                     kCFAllocatorDefault, 0);

                        ////// Next info
                        //.....
                        io_registry_entry_t currParent = parent;
                        kernResult = IORegistryEntryGetParentEntry(currParent, kIOServicePlane, &parent);
                        IOObjectRelease(currParent);
                    }

                    QByteArray stringValue(MAXPATHLEN, 0);
                    //
                    if (descriptionRef) {
                        if (CFStringGetCString(CFStringRef(descriptionRef), stringValue.data(), stringValue.size(), kCFStringEncodingUTF8)) {
                            si.description = QString(stringValue);
                            si.shortName = QString(bsdPath); //FIXME:
                            si.friendlyName = si.description + "(" + si.shortName + ")";
                        }
                        CFRelease(descriptionRef);
                    }
                    //
                    if (manufacturerRef) {
                        if (CFStringGetCString(CFStringRef(manufacturerRef), stringValue.data(), stringValue.size(), kCFStringEncodingUTF8)) {
                            si.manufacturer = QString(stringValue);
                        }
                        CFRelease(manufacturerRef);
                    }
                    //
                    if (busRef) {
                        if (CFStringGetCString(CFStringRef(busRef), stringValue.data(), stringValue.size(), kCFStringEncodingUTF8)) {
                            si.bus = QString(stringValue);
                        }
                        CFRelease(busRef);
                    }
                    //
                    if (subsysRef) {
                        if (CFStringGetCString(CFStringRef(subsysRef), stringValue.data(), stringValue.size(), kCFStringEncodingUTF8)) {
                            si.subSystem = QString(stringValue);
                        }
                        CFRelease(subsysRef);
                    }
                    //
                    if (driverRef) {
                        if (CFStringGetCString(CFStringRef(driverRef), stringValue.data(), stringValue.size(), kCFStringEncodingUTF8)) {
                            si.driverName = QString(stringValue);
                        }
                        CFRelease(driverRef);
                    }
                    //
                    if (serviceRef) {
                        if (CFStringGetCString(CFStringRef(serviceRef), stringValue.data(), stringValue.size(), kCFStringEncodingUTF8)) {
                            si.service = QString(stringValue);
                        }
                        CFRelease(serviceRef);
                    }

                    // Here anothes string properties.
                    //...

                    int numericValue = 0;
                    //
                    if (vidRef) {
                        if (CFNumberGetValue(CFNumberRef(vidRef), kCFNumberIntType, &numericValue)) {
                            si.vendorID = QString::number(numericValue, 16);
                        }
                        CFRelease(vidRef);
                    }
                    //
                    if (pidRef) {
                        if (CFNumberGetValue(CFNumberRef(pidRef), kCFNumberIntType, &numericValue)) {
                            si.productID = QString::number(numericValue, 16);
                        }
                        CFRelease(pidRef);
                    }

                    info[QString(bsdPath)] = si;
                }
                (void) IOObjectRelease(serialService);
            }
        }
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
}

bool SerialDeviceEnumeratorPrivate::isValid() const
{
    return (this->classesToMatch && this->notifier && this->loop && this->enabled);
}

void SerialDeviceEnumeratorPrivate::notifierHandler()
{
    this->_q_processWatcher();
}
