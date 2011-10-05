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

#include <QtCore/QAbstractEventDispatcher>

#include "nativeserialengine.h"
#include "nativeserialengine_p.h"
#include "abstractserialnotifier.h"


//#define NATIVESERIALENGINE_DEBUG

#ifdef NATIVESERIALENGINE_DEBUG
#include <QtCore/QDebug>
#endif


//-----------------------------------------------------------------------------------------------//

NativeSerialEngine::NativeSerialEngine(QObject *parent)
        : AbstractSerialEngine(*new NativeSerialEnginePrivate(), parent)
{
}

NativeSerialEngine::~NativeSerialEngine()
{
    this->close();//?
}

bool NativeSerialEngine::open(AbstractSerial::OpenMode mode)
{
    Q_D(NativeSerialEngine);
    bool ret = d->nativeOpen(mode);
    if (!ret)
        d->nativeClose();
    return ret;
}

void NativeSerialEngine::close()
{
    Q_D(NativeSerialEngine);
    d->nativeClose();
}

bool NativeSerialEngine::setBaudRate(qint32 baudRate, AbstractSerial::BaudRateDirection baudDir)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetBaudRate(baudRate, baudDir);
}

bool NativeSerialEngine::setDataBits(AbstractSerial::DataBits dataBits)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetDataBits(dataBits);
}

bool NativeSerialEngine::setParity(AbstractSerial::Parity parity)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetParity(parity);
}

bool NativeSerialEngine::setStopBits(AbstractSerial::StopBits stopBits)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetStopBits(stopBits);
}

bool NativeSerialEngine::setFlowControl(AbstractSerial::Flow flow)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetFlowControl(flow);
}

void NativeSerialEngine::setCharReadTimeout(int usecs)
{
    Q_D(NativeSerialEngine);
    d->nativeSetCharReadTimeout(usecs);
}

int NativeSerialEngine::charReadTimeout() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeCharReadTimeout();
}

void NativeSerialEngine::setTotalReadConstantTimeout(int msecs)
{
    Q_D(NativeSerialEngine);
    d->nativeSetTotalReadConstantTimeout(msecs);
}

int NativeSerialEngine::totalReadConstantTimeout() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeTotalReadConstantTimeout();
}

bool NativeSerialEngine::setDtr(bool set) const
{
    Q_D(const NativeSerialEngine);
    return d->nativeSetDtr(set);
}

bool NativeSerialEngine::setRts(bool set) const
{
    Q_D(const NativeSerialEngine);
    return d->nativeSetRts(set);
}

quint16 NativeSerialEngine::lineStatus() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeLineStatus();
}

bool NativeSerialEngine::sendBreak(int duration) const
{
    Q_D(const NativeSerialEngine);
    return d->nativeSendBreak(duration);
}

bool NativeSerialEngine::setBreak(bool set) const
{
    Q_D(const NativeSerialEngine);
    return d->nativeSetBreak(set);
}

bool NativeSerialEngine::flush() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeFlush();
}

bool NativeSerialEngine::reset() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeReset();
}

qint64 NativeSerialEngine::bytesAvailable() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeBytesAvailable();
}

qint64 NativeSerialEngine::write(const char *data, qint64 maxSize)
{
    Q_D(NativeSerialEngine);
    qint64 ret = 0;
    forever {
        qint64 bytesToWrite = qMin<qint64>(WRITE_CHUNKSIZE, maxSize - ret);
        qint64 bytesWritten = d->nativeWrite((const char*)(data + ret), bytesToWrite);

        if ((bytesWritten <= 0) || (bytesWritten != bytesToWrite))
            return qint64(-1);

        ret += bytesWritten;

        if (ret == maxSize)
            break;
    }
    return ret;
}

qint64 NativeSerialEngine::read(char *data, qint64 maxSize)
{
    Q_D(NativeSerialEngine);
    qint64 ret = 0;
    forever {
        qint64 bytesToRead = qMin<qint64>(READ_CHUNKSIZE, maxSize - ret);
        qint64 bytesReaded = d->nativeRead(data + ret, bytesToRead);

        if (bytesReaded <= 0)
            break;

        ret += bytesReaded;
        if ((ret == maxSize) || (bytesReaded < bytesToRead))
            break;
    }
    return ret;
}

bool NativeSerialEngine::waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
                                            bool checkRead, bool checkWrite,
                                            int msecs)
{
    Q_D(NativeSerialEngine);
    int ret = d->nativeSelect(msecs, checkRead, checkWrite, readyToRead, readyToWrite);
    return (ret > 0);
}

bool NativeSerialEngine::isReadNotificationEnabled() const
{
    Q_D(const NativeSerialEngine);
    return (d->notifier
            && d->notifier->isReadNotificationEnabled());
}

void NativeSerialEngine::setReadNotificationEnabled(bool enable, bool onClose)
{
    Q_D(NativeSerialEngine);

    if (onClose)
        enable = false;

    if (!d->notifier && enable && QAbstractEventDispatcher::instance(thread()))
            d->notifier = AbstractSerialNotifier::createSerialNotifier(this);

    if (d->notifier)
        d->notifier->setReadNotificationEnabled(enable);
    if (onClose && d->notifier)
        this->clearNotification();
}

bool NativeSerialEngine::isWriteNotificationEnabled() const
{
    Q_D(const NativeSerialEngine);
    return (d->notifier
            && d->notifier->isWriteNotificationEnabled());
}

void NativeSerialEngine::setWriteNotificationEnabled(bool enable, bool onClose)
{
    Q_D(NativeSerialEngine);

    if (onClose)
        enable = false;

    if (!d->notifier && enable && QAbstractEventDispatcher::instance(thread()))
            d->notifier = AbstractSerialNotifier::createSerialNotifier(this);

    if (d->notifier)
        d->notifier->setWriteNotificationEnabled(enable);
    if (onClose && d->notifier)
        this->clearNotification();
}

bool NativeSerialEngine::isExceptionNotificationEnabled() const
{
    Q_D(const NativeSerialEngine);
    return (d->notifier
            && d->notifier->isExceptionNotificationEnabled());
}

void NativeSerialEngine::setExceptionNotificationEnabled(bool enable, bool onClose)
{
    Q_D(NativeSerialEngine);

    if (onClose)
        enable = false;

    if (!d->notifier && enable && QAbstractEventDispatcher::instance(thread()))
            d->notifier = AbstractSerialNotifier::createSerialNotifier(this);

    if (d->notifier)
        d->notifier->setExceptionNotificationEnabled(enable);
    if (onClose && d->notifier)
        this->clearNotification();
}

bool NativeSerialEngine::isLineNotificationEnabled() const
{
    Q_D(const NativeSerialEngine);
    return (d->notifier
            && d->notifier->isLineNotificationEnabled());
}

void NativeSerialEngine::setLineNotificationEnabled(bool enable, bool onClose)
{
    Q_D(NativeSerialEngine);

    if (onClose)
        enable = false;

    if (!d->notifier && enable && QAbstractEventDispatcher::instance(thread()))
            d->notifier = AbstractSerialNotifier::createSerialNotifier(this);

    if (d->notifier)
        d->notifier->setLineNotificationEnabled(enable);
    if (onClose && d->notifier)
        this->clearNotification();
}

void NativeSerialEngine::clearNotification()
{
    Q_D(NativeSerialEngine);

    AbstractSerialNotifier::deleteSerialNotifier(d->notifier);
    d->notifier = 0;
}

//add 05.11.2009  (while is not used, reserved)
qint64 NativeSerialEngine::currentTxQueue() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeCurrentQueue(txQueue);
}

//add 05.11.2009  (while is not used, reserved)
qint64 NativeSerialEngine::currentRxQueue() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeCurrentQueue(rxQueue);
}


#include "moc_nativeserialengine.cpp"
