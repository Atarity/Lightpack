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

#ifndef NATIVESERIALENGINE_H
#define NATIVESERIALENGINE_H

#include "abstractserialengine.h"



class NativeSerialEnginePrivate;
class NativeSerialEngine : public AbstractSerialEngine
{
    Q_OBJECT
public:
    explicit NativeSerialEngine(QObject *parent = 0);
    virtual ~NativeSerialEngine();

    bool open(AbstractSerial::OpenMode mode);
    void close();

    bool setBaudRate(qint32 baudRate, AbstractSerial::BaudRateDirection baudDir);
    bool setDataBits(AbstractSerial::DataBits dataBits);
    bool setParity(AbstractSerial::Parity parity);
    bool setStopBits(AbstractSerial::StopBits stopBits);
    bool setFlowControl(AbstractSerial::Flow flow);

    void setCharReadTimeout(int usecs);
    int charReadTimeout() const;

    void setTotalReadConstantTimeout(int msecs);
    int totalReadConstantTimeout() const;

    bool setDtr(bool set) const;
    bool setRts(bool set) const;
    quint16 lineStatus() const;

    bool sendBreak(int duration) const;
    bool setBreak(bool set) const;

    bool flush() const;
    bool reset() const;

    qint64 bytesAvailable() const;

    qint64 read(char *data, qint64 maxSize);
    qint64 write(const char *data, qint64 maxSize);

    bool waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
                            bool checkRead, bool checkWrite,
                            int msecs = 3000);

    bool isReadNotificationEnabled() const;
    void setReadNotificationEnabled(bool enable, bool onClose = false);
    bool isWriteNotificationEnabled() const;
    void setWriteNotificationEnabled(bool enable, bool onClose = false);
    bool isExceptionNotificationEnabled() const;
    void setExceptionNotificationEnabled(bool enable, bool onClose = false);
    bool isLineNotificationEnabled() const;
    void setLineNotificationEnabled(bool enable, bool onClose = false);

    void clearNotification();

protected:

    //add 05.11.2009
    enum ioQueue{ txQueue, rxQueue };
    qint64 currentTxQueue() const;
    qint64 currentRxQueue() const;

private:
    Q_DECLARE_PRIVATE(NativeSerialEngine)
    Q_DISABLE_COPY(NativeSerialEngine)
};

#endif // NATIVESERIALENGINE_H
