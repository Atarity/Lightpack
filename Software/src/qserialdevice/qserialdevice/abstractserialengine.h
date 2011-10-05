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

#ifndef ABSTRACTSERIALENGINE_H
#define ABSTRACTSERIALENGINE_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "abstractserial.h"

/* Serial::DESCRIPTOR
    Serial Device descriptor namespace.
*/
namespace Serial {
#if defined (Q_OS_UNIX)
    typedef int DESCRIPTOR;
#elif defined(Q_OS_WIN32)
    typedef void *DESCRIPTOR;
#endif
}

class AbstractSerialEnginePrivate;

class AbstractSerialEngineReceiver {
public:
    virtual ~AbstractSerialEngineReceiver() {}
    virtual void readNotification()= 0;
    virtual void writeNotification()= 0;
    virtual void exceptionNotification()= 0;
    virtual void lineNotification()= 0;
};

class AbstractSerialEngine : public QObject
{
    Q_OBJECT

public:
    explicit AbstractSerialEngine(QObject *parent = 0);
    virtual ~AbstractSerialEngine();

    static AbstractSerialEngine *createSerialEngine(QObject *parent);

    void setDeviceName(const QString &deviceName);
    QString deviceName() const;

    Serial::DESCRIPTOR descriptor() const;

    virtual bool open(QIODevice::OpenMode mode) = 0;
    virtual void close() = 0;

    virtual bool setBaudRate(qint32 baudRate, AbstractSerial::BaudRateDirection baudDir) = 0;
    qint32 baudRate(AbstractSerial::BaudRateDirection baudDir) const;

    virtual bool setDataBits(AbstractSerial::DataBits dataBits) = 0;
    AbstractSerial::DataBits dataBits() const;

    virtual bool setParity(AbstractSerial::Parity parity) = 0;
    AbstractSerial::Parity parity() const;

    virtual bool setStopBits(AbstractSerial::StopBits stopBits) = 0;
    AbstractSerial::StopBits stopBits() const;

    virtual bool setFlowControl(AbstractSerial::Flow flow) = 0;
    AbstractSerial::Flow flow() const;

    virtual void setCharReadTimeout(int usecs) = 0;
    virtual int charReadTimeout() const = 0;

    virtual void setTotalReadConstantTimeout(int msecs) = 0;
    virtual int totalReadConstantTimeout() const = 0;

    virtual bool setDtr(bool set) const = 0;
    virtual bool setRts(bool set) const = 0;
    virtual quint16 lineStatus() const = 0;

    virtual bool sendBreak(int duration) const = 0;
    virtual bool setBreak(bool set) const = 0;

    virtual bool flush() const = 0;
    virtual bool reset() const = 0;

    virtual qint64 bytesAvailable() const = 0;

    virtual qint64 read(char *data, qint64 maxSize) = 0;
    virtual qint64 write(const char *data, qint64 maxSize) = 0;

    virtual bool waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
                                    bool checkRead, bool checkWrite,
                                    int msecs = 3000) = 0;

    AbstractSerial::Status status() const;

    virtual bool isReadNotificationEnabled() const = 0;
    virtual void setReadNotificationEnabled(bool enable, bool onClose = false) = 0;
    virtual bool isWriteNotificationEnabled() const = 0;
    virtual void setWriteNotificationEnabled(bool enable, bool onClose = false) = 0;
    virtual bool isExceptionNotificationEnabled() const = 0;
    virtual void setExceptionNotificationEnabled(bool enable, bool onClose = false) = 0;
    virtual bool isLineNotificationEnabled() const = 0;
    virtual void setLineNotificationEnabled(bool enable, bool onClose = false) = 0;

    void setReceiver(AbstractSerialEngineReceiver *receiver);

public Q_SLOTS:
    void readNotification();
    void writeNotification();
    void exceptionNotification();
    void lineNotification();

protected:
    AbstractSerialEngine(AbstractSerialEnginePrivate &dd, QObject *parent);
    AbstractSerialEnginePrivate * const d_ptr;

    //add 05.11.2009 (while is not used, reserved)
    virtual qint64 currentTxQueue() const = 0;
    virtual qint64 currentRxQueue() const = 0;

private:
    Q_DECLARE_PRIVATE(AbstractSerialEngine)
    Q_DISABLE_COPY(AbstractSerialEngine)
};

#endif // ABSTRACTSERIALENGINE_H
