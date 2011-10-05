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

#ifndef SERIALDEVICEENUMERATOR_H
#define SERIALDEVICEENUMERATOR_H

#include <QtCore/QObject>

#include "../qserialdevice_global.h"

class SerialDeviceEnumeratorPrivate;
#if defined(QSERIALDEVICE_EXPORT)
class QSERIALDEVICE_EXPORT SerialDeviceEnumerator : public QObject
#else
class SerialDeviceEnumerator : public QObject
#endif
{
    Q_OBJECT

Q_SIGNALS:
    void hasChanged(const QStringList &list);

public:

    static SerialDeviceEnumerator *instance();

    void setEnabled(bool enable);
    bool isEnabled() const;

    QStringList devicesAvailable() const;

    //Info methods
    void setDeviceName(const QString &name);
    //
    QString name() const;
    QString shortName() const;
    QString systemPath() const;
    QString subSystem() const;
    QString locationInfo() const;
    QString driver() const;
    QString friendlyName() const;
    QString description() const;
    QStringList hardwareID() const;
    QString vendorID() const;
    QString productID() const;
    QString manufacturer() const;
    QString service() const;
    //
    QString bus() const;
    QString revision() const;
    //
    bool isExists() const;
    bool isBusy() const;

protected:
    SerialDeviceEnumeratorPrivate * const d_ptr;

private:
    explicit SerialDeviceEnumerator(QObject *parent = 0);
    virtual ~SerialDeviceEnumerator();

    static SerialDeviceEnumerator *self;

    Q_DECLARE_PRIVATE(SerialDeviceEnumerator)
    Q_DISABLE_COPY(SerialDeviceEnumerator)
    Q_PRIVATE_SLOT(d_func(),void _q_processWatcher())
};

#endif // SERIALDEVICEENUMERATOR_H
