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

#include <QtCore/QRegExp>

#include "nativeserialengine.h"
#include "abstractserialengine_p.h"


AbstractSerialEnginePrivate::AbstractSerialEnginePrivate()
        : deviceName(QString())
        , ibaudRate(AbstractSerial::BaudRate9600)
        , obaudRate(AbstractSerial::BaudRate9600)
        , dataBits(AbstractSerial::DataBits8)
        , parity(AbstractSerial::ParityNone)
        , stopBits(AbstractSerial::StopBits1)
        , flow(AbstractSerial::FlowControlOff)
        , charIntervalTimeout(0)
        , status(AbstractSerial::ENone)
        , oldSettingsIsSaved(false)
        , descriptor(0)
        , receiver(0)
        , isAutoCalcReadTimeoutConstant(false)
{
}

#ifdef Q_OS_WIN32
    const char AbstractSerialEnginePrivate::defaultDeviceName[] = "COM1";
#elif defined(Q_OS_IRIX)
    const char AbstractSerialEnginePrivate::defaultDeviceName[] = "/dev/ttyf1";
#elif defined(Q_OS_HPUX)
    const char AbstractSerialEnginePrivate::defaultDeviceName[] = "/dev/tty1p0";
#elif defined(Q_OS_SOLARIS)
    const char AbstractSerialEnginePrivate::defaultDeviceName[] = "/dev/ttya";
#elif defined(Q_OS_FREEBSD)
    const char AbstractSerialEnginePrivate::defaultDeviceName[] = "/dev/ttyd1";
#elif defined(Q_OS_LINUX)
    const char AbstractSerialEnginePrivate::defaultDeviceName[] = "/dev/ttyS0";
#else
    const char AbstractSerialEnginePrivate::defaultDeviceName[] = "/dev/ttyS0";
#endif


int AbstractSerialEnginePrivate::currDataBitsToValue() const
{
    switch (this->dataBits) {
    case AbstractSerial::DataBits5: return 5;
    case AbstractSerial::DataBits6: return 6;
    case AbstractSerial::DataBits7: return 7;
    case AbstractSerial::DataBits8: return 8;
    default:;
    }
    return 8;
}

int AbstractSerialEnginePrivate::currParityToValue() const
{
    switch (this->parity) {
    case AbstractSerial::ParityNone: return 0;
    default:;
    }
    return 1;
}

int AbstractSerialEnginePrivate::currStopBitsToValue() const
{
    switch (this->stopBits) {
    case AbstractSerial::StopBits2: return 2;
    default:;
    }
    return 1;
}

//---------------------------------------------------------------------------//

AbstractSerialEngine::AbstractSerialEngine(QObject *parent)
    : QObject(parent)
    , d_ptr(new AbstractSerialEnginePrivate())
{
}

AbstractSerialEngine::AbstractSerialEngine(AbstractSerialEnginePrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
}

AbstractSerialEngine::~AbstractSerialEngine()
{
    delete d_ptr;
}

AbstractSerialEngine *AbstractSerialEngine::createSerialEngine(QObject *parent)
{
    return new NativeSerialEngine(parent);
}

void AbstractSerialEngine::setDeviceName(const QString &deviceName)
{
    d_func()->deviceName = deviceName;
}

QString AbstractSerialEngine::deviceName() const
{
    return d_func()->deviceName;
}

Serial::DESCRIPTOR AbstractSerialEngine::descriptor() const
{
    return d_func()->descriptor;
}

qint32 AbstractSerialEngine::baudRate(AbstractSerial::BaudRateDirection baudDir) const
{
    switch (baudDir) {
    case AbstractSerial::InputBaud: return d_func()->ibaudRate;
    case AbstractSerial::OutputBaud: return d_func()->obaudRate;
    default:;
    }
    return (d_func()->ibaudRate == d_func()->obaudRate) ? d_func()->ibaudRate : 0;
}

AbstractSerial::DataBits AbstractSerialEngine::dataBits() const
{
    return d_func()->dataBits;
}

AbstractSerial::Parity AbstractSerialEngine::parity() const
{
    return d_func()->parity;
}

AbstractSerial::StopBits AbstractSerialEngine::stopBits() const
{
    return d_func()->stopBits;
}

AbstractSerial::Flow AbstractSerialEngine::flow() const
{
    return d_func()->flow;
}

AbstractSerial::Status AbstractSerialEngine::status() const
{
    return d_func()->status;
}

void AbstractSerialEngine::setReceiver(AbstractSerialEngineReceiver *receiver)
{
    d_func()->receiver = receiver;
}

void AbstractSerialEngine::readNotification()
{
    if (AbstractSerialEngineReceiver *receiver = d_func()->receiver)
        receiver->readNotification();
}

void AbstractSerialEngine::writeNotification()
{
    if (AbstractSerialEngineReceiver *receiver = d_func()->receiver)
        receiver->writeNotification();
}

void AbstractSerialEngine::exceptionNotification()
{
    if (AbstractSerialEngineReceiver *receiver = d_func()->receiver)
        receiver->exceptionNotification();
}

void AbstractSerialEngine::lineNotification()
{
    if (AbstractSerialEngineReceiver *receiver = d_func()->receiver)
        receiver->lineNotification();
}

#include "moc_abstractserialengine.cpp"
