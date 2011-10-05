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

#include "nativeserialengine_p.h"

#ifndef Q_CC_MSVC
#  include <ddk/ntddser.h>
#else
#ifndef IOCTL_SERIAL_GET_DTRRTS
#define IOCTL_SERIAL_GET_DTRRTS \
    CTL_CODE (FILE_DEVICE_SERIAL_PORT, 30, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif //Q_CC_MSVC

#ifndef SERIAL_DTR_STATE
#  define SERIAL_DTR_STATE                  0x00000001
#endif

#ifndef SERIAL_RTS_STATE
#  define SERIAL_RTS_STATE                  0x00000002
#endif

#endif

//#define NATIVESERIALENGINE_WIN_DEBUG

#ifdef NATIVESERIALENGINE_WIN_DEBUG
#include <QtCore/QDebug>
#endif


NativeSerialEnginePrivate::NativeSerialEnginePrivate()
{
    this->initVariables();
}

bool NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode)
{
    ::DWORD access = 0;
    ::DWORD sharing = 0;
    bool rx = false;
    bool tx = false;

    if (QIODevice::ReadOnly & mode) {
        access |= GENERIC_READ; //sharing = FILE_SHARE_READ;
        rx = true;
    }
    if (QIODevice::WriteOnly & mode) {
        access |= GENERIC_WRITE; //sharing = FILE_SHARE_WRITE;
        tx = true;
    }

    //try opened serial device
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
            " -> trying to open device: " << this->deviceName << "\n";
#endif

    //Create the file handle.
    QString path = "\\\\.\\" + this->deviceName;
    QByteArray nativeFilePath = QByteArray((const char *)path.utf16(), path.size() * 2 + 1);

    this->descriptor = ::CreateFile((const wchar_t*)nativeFilePath.constData(),
                                    access,
                                    sharing,
                                    0,
                                    OPEN_EXISTING,
                                    FILE_FLAG_OVERLAPPED,
                                    0);

    if (INVALID_HANDLE_VALUE == this->descriptor) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: ::CreateFile(...) returned: " << this->descriptor << ", \n"
                " last error is: " << ::GetLastError() << ". Error! \n";
#endif
        return false;
    }

    if (!this->saveOldSettings()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: saveOldSettings() returned: false. Error! \n";
#endif
        return false;
    }

    //Prepare other options
    this->prepareOtherOptions();

    if (!this->updateDcb()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: ::updateDcb() returned: false. Error! \n";
#endif
        return false;
    }

    // Prepare timeouts.
    this->prepareCommTimeouts(NativeSerialEnginePrivate::ReadIntervalTimeout, MAXWORD);
    this->prepareCommTimeouts(NativeSerialEnginePrivate::ReadTotalTimeoutMultiplier, 0);
    this->prepareCommTimeouts(NativeSerialEnginePrivate::ReadTotalTimeoutConstant, 0);
    this->prepareCommTimeouts(NativeSerialEnginePrivate::WriteTotalTimeoutMultiplier, 0);
    this->prepareCommTimeouts(NativeSerialEnginePrivate::WriteTotalTimeoutConstant, 0);

    if (!this->updateCommTimeouts()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: updateCommTimeouts() returned: false. Error! \n";
#endif
        return false;
    }

    // Disable autocalculate total read interval.
    this->isAutoCalcReadTimeoutConstant = false;

    if (!this->detectDefaultCurrentSettings()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: detectDefaultCurrentSettings() returned: false. Error! \n";
#endif
        return false;
    }

    if (!this->createEvents(rx, tx)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: createEvents(eRx, eTx) returned: false. Error! \n";
#endif
        return false;
    }

#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
            " -> opened device: " << this->deviceName << " in mode: " << mode << " succesfully. Ok! \n";
#endif

    return true;
}

bool NativeSerialEnginePrivate::nativeClose()
{
    bool closeResult = true;

    if (INVALID_HANDLE_VALUE == this->descriptor)
        return closeResult;

    //try restore old parameters device
    if (!this->restoreOldSettings()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeClose() \n"
               " -> function: restoreOldSettings() returned: false. Error! \n");
#endif
        //closeResult = false;
    }

    ::CancelIo(this->descriptor);
    //try closed device
    if (0 == ::CloseHandle(this->descriptor)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeClose() \n"
               " -> function: ::CloseHandle(this->descriptor) returned: 0. Error! \n");
#endif
        //closeResult = false;
    }

    if (!this->closeEvents())
        closeResult = false;

    //if (closeResult) {
        this->descriptor = INVALID_HANDLE_VALUE;
    //}

    return closeResult;
}

bool NativeSerialEnginePrivate::nativeSetBaudRate(qint32 baudRate, AbstractSerial::BaudRateDirection baudDir)
{
    if (AbstractSerial::AllBaud != baudDir)
        return false;

    this->dcb.BaudRate = ::DWORD(baudRate);
    if (!this->updateDcb()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetBaudRate(quint32 baudRate) \n"
                " -> function: ::updateDcb() returned: false. Error! \n";
#endif
        return false;
    }

    this->ibaudRate = this->obaudRate = baudRate;//!!!
    this->recalcTotalReadTimeoutConstant();
    return true;
}

bool NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits)
{
    if ((AbstractSerial::DataBits5 == dataBits)
        && (AbstractSerial::StopBits2 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
               " -> 5 data bits cannot be used with 2 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits6 == dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
               " -> 6 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits7 == dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
               " -> 7 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits8 == dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
               " -> 8 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    switch (dataBits) {
    case AbstractSerial::DataBits5: this->dcb.ByteSize = 5; break;
    case AbstractSerial::DataBits6: this->dcb.ByteSize = 6; break;
    case AbstractSerial::DataBits7: this->dcb.ByteSize = 7; break;
    case AbstractSerial::DataBits8: this->dcb.ByteSize = 8; break;
    default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
                " -> dataBits: " << dataBits << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::DataBits. Error! \n";
#endif
        return false;
    }

    if (!this->updateDcb()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
                " -> function: ::updateDcb() returned: false. Error! \n";
#endif
        return false;
    }

    this->dataBits = dataBits;
    this->recalcTotalReadTimeoutConstant();
    return true;
}

bool NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity)
{
    switch (parity) {
    case AbstractSerial::ParityNone: this->dcb.Parity = NOPARITY; this->dcb.fParity = false; break;
    case AbstractSerial::ParitySpace: this->dcb.Parity = SPACEPARITY; this->dcb.fParity = true; break;
    case AbstractSerial::ParityMark: this->dcb.Parity = MARKPARITY; this->dcb.fParity = true; break;
    case AbstractSerial::ParityEven: this->dcb.Parity = EVENPARITY; this->dcb.fParity = true; break;
    case AbstractSerial::ParityOdd: this->dcb.Parity = ODDPARITY; this->dcb.fParity = true; break;
    default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> parity: " << parity << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::Parity. Error! \n";
#endif
        return false;
    }

    if (!this->updateDcb()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> function: ::updateDcb() returned: false. Error! \n";
#endif
        return false;
    }

    this->parity = parity;
    this->recalcTotalReadTimeoutConstant();
    return true;
}

bool NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits)
{
    if ((AbstractSerial::DataBits5 == this->dataBits)
        && (AbstractSerial::StopBits2 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
               " -> 5 data bits cannot be used with 2 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits6 == this->dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
               " -> 6 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits7 == this->dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
               " -> 7 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits8 == this->dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
               " -> 8 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    switch (stopBits) {
    case AbstractSerial::StopBits1: this->dcb.StopBits = ONESTOPBIT; break;
    case AbstractSerial::StopBits1_5: this->dcb.StopBits = ONE5STOPBITS; break;
    case AbstractSerial::StopBits2: this->dcb.StopBits = TWOSTOPBITS; break;
    default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
                " -> stopBits: " << stopBits << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::StopBits. Error! \n";
#endif
        return false;
    }

    if (!this->updateDcb()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
                " -> function: ::updateDcb() returned: false. Error! \n";
#endif
        return false;
    }

    this->stopBits = stopBits;
    this->recalcTotalReadTimeoutConstant();
    return true;
}

bool NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow)
{
    switch (flow) {
    case AbstractSerial::FlowControlOff:
        this->dcb.fOutxCtsFlow = false; this->dcb.fRtsControl = RTS_CONTROL_DISABLE;
        this->dcb.fInX = this->dcb.fOutX = false; break;
    case AbstractSerial::FlowControlXonXoff:
        this->dcb.fOutxCtsFlow = false; this->dcb.fRtsControl = RTS_CONTROL_DISABLE;
        this->dcb.fInX = this->dcb.fOutX = true; break;
    case AbstractSerial::FlowControlHardware:
        this->dcb.fOutxCtsFlow = true; this->dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        this->dcb.fInX = this->dcb.fOutX = false; break;
    default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
                " -> flow: " << flow << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::Flow. Error! \n";
#endif
        return false;
    }

    if (!this->updateDcb()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
                " -> function: ::SetCommState(this->descriptor, this->dcb) returned: 0. Error! \n";
#endif
        return false;
    }

    this->flow = flow;
    return true;
}

void NativeSerialEnginePrivate::nativeSetCharReadTimeout(int usecs)
{
    this->charIntervalTimeout = usecs;
    this->recalcTotalReadTimeoutConstant();
}

int NativeSerialEnginePrivate::nativeCharReadTimeout() const
{
    return this->charIntervalTimeout;
}

void NativeSerialEnginePrivate::nativeSetTotalReadConstantTimeout(int msecs)
{
    switch(msecs) {
    case -1:
        this->isAutoCalcReadTimeoutConstant = true;
        this->recalcTotalReadTimeoutConstant();
        break;
    default:
        this->isAutoCalcReadTimeoutConstant = false;
        this->prepareCommTimeouts(NativeSerialEnginePrivate::ReadTotalTimeoutConstant,
                                  ::DWORD(msecs));
        this->updateCommTimeouts();
    }
}

int NativeSerialEnginePrivate::nativeTotalReadConstantTimeout() const
{
    return quint32(this->ct.ReadTotalTimeoutConstant);
}

bool NativeSerialEnginePrivate::nativeSetDtr(bool set) const
{
    bool result = ::EscapeCommFunction(this->descriptor, (set) ? SETDTR : CLRDTR);
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    if (!result) {
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetDtr(bool set) \n"
               " -> function: ::EscapeCommFunction(...) returned: false. Error! \n");
    }
#endif
    return result;
}

bool NativeSerialEnginePrivate::nativeSetRts(bool set) const
{
    bool result = ::EscapeCommFunction(this->descriptor, (set) ? SETRTS : CLRRTS);
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    if (!result) {
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
               " -> function: ::EscapeCommFunction(...) returned: false. Error! \n");
    }
#endif
    return result;
}

quint16 NativeSerialEnginePrivate::nativeLineStatus() const
{
    ::DWORD temp = 0;
    quint16 status = AbstractSerial::LineErr;

    if (0 == ::GetCommModemStatus(this->descriptor, &temp)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeLineStatus() \n"
               " -> function: ::GetCommModemStatus(...) returned: 0. Error! \n");
#endif
        return status;
    }

    status = 0;
    if (temp & MS_CTS_ON) status |= AbstractSerial::LineCTS;
    if (temp & MS_DSR_ON) status |= AbstractSerial::LineDSR;
    if (temp & MS_RING_ON) status |= AbstractSerial::LineRI;
    if (temp & MS_RLSD_ON) status |= AbstractSerial::LineDCD;

    // Get DTR/RTS line states,
    // where variable temp return states.
    ::DWORD bytesReturned = 0;
    if (::DeviceIoControl(this->descriptor,
                          IOCTL_SERIAL_GET_DTRRTS,
                          0,
                          0,
                          &temp,
                          sizeof(::DWORD),
                          &bytesReturned,
                          0)) {

        if (temp & SERIAL_DTR_STATE) status |= AbstractSerial::LineDTR;
        if (temp & SERIAL_RTS_STATE) status |= AbstractSerial::LineRTS;
    }

    return status;
}

bool NativeSerialEnginePrivate::nativeSendBreak(int duration) const
{
    //TODO: help implementation?!
    if (this->nativeSetBreak(true)) {
        ::Sleep(::DWORD(duration));
        if (this->nativeSetBreak(false))
            return true;
    }
    return false;
}

bool NativeSerialEnginePrivate::nativeSetBreak(bool set) const
{
    if (set) {
        if (0 == ::SetCommBreak(this->descriptor)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
            qDebug("Windows: NativeSerialEnginePrivate::nativeSetBreak(bool set) \n"
                   " -> function: ::SetCommBreak(this->descriptor) returned: 0. Error! \n");
#endif
            return false;
        }
        return true;
    }

    if (0 == ::ClearCommBreak(this->descriptor)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeSetBreak(bool set) \n"
               " -> function: ::ClearCommBreak(this->descriptor) returned: 0. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeFlush() const
{
    bool ret = ::FlushFileBuffers(this->descriptor);
    if (!ret) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeFlush() \n"
               " -> function:: ::FlushFileBuffers(this->descriptor) returned: false. Error! \n");
#endif
        ;
    }
    return ret;
}

bool NativeSerialEnginePrivate::nativeReset() const
{
    bool ret = true;
    ::DWORD flags = (PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    if (0 == ::PurgeComm(this->descriptor, flags)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeReset() \n"
               " -> function: ::PurgeComm(...) returned: 0. Error! \n");
#endif
        ret = false;
    }
    return ret;
}

qint64 NativeSerialEnginePrivate::nativeBytesAvailable() const
{
    ::DWORD err = 0;
    ::COMSTAT cs = {0};
    qint64 available = -1;

    if (0 == ::ClearCommError(this->descriptor, &err, &cs)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Windows: NativeSerialEnginePrivate::nativeBytesAvailable(bool wait) \n"
               " -> function: ::ClearCommError(this->descriptor, &err, &cs) returned: 0. Error! \n");
#endif
        return available;
    }
    if (err) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeBytesAvailable(bool wait) \n"
                " -> in function: ::ClearCommError(this->descriptor, &err, &cs), \n"
                " output parameter: err: " << err << ". Error! \n";
#endif
        return available;
    }
    available = qint64(cs.cbInQue);
    return available;
}

/* Clear overlapped structure, but does not affect the event.
   If Event exists then return True.
*/
static bool clear_overlapped(::OVERLAPPED *o)
{
    o->Internal = o->InternalHigh = o->Offset = o->OffsetHigh = 0;
    return (0 != o->hEvent);
}

qint64 NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    if (!clear_overlapped(&this->oWrite))
        return qint64(-1);

    ::DWORD writeBytes = 0;
    bool sucessResult = false;

    if (::WriteFile(this->descriptor, (LPCVOID)data, (DWORD)len, &writeBytes, &this->oWrite))
        sucessResult = true;
    else {
        ::DWORD rc = ::GetLastError();
        if (ERROR_IO_PENDING == rc) {
            //not to loop the function, instead of setting INFINITE put 5000 milliseconds wait!
            rc = ::WaitForSingleObject(this->oWrite.hEvent, 5000);
            switch (rc) {
            case WAIT_OBJECT_0:
                if (::GetOverlappedResult(this->descriptor, &this->oWrite, &writeBytes, false))
                    sucessResult = true;
                else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
                    qDebug("Windows: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) \n"
                           " -> function: ::GetOverlappedResult(this->descriptor, &this->oWrite, &writeBytes, false) returned: false. Error! \n");
#endif
                    ;
                }
                break;//WAIT_OBJECT_0
            default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
                qDebug() << "Windows: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) \n"
                        " -> function: ::::WaitForSingleObject(this->oWrite.hEvent, 5000) returned: " << rc << ". Error! \n";
#endif
                ;
            }//switch (rc)
        }
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
            qDebug() << "Windows: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) \n"
                    " -> function: ::GetLastError() returned: " << rc << ". Error! \n";
#endif
            ;
        }
    }

    return (sucessResult) ? quint64(writeBytes) : qint64(-1);
}

qint64 NativeSerialEnginePrivate::nativeRead(char *data, qint64 len)
{
    if (!clear_overlapped(&this->oRead))
        return qint64(-1);

    ::DWORD readBytes = 0;
    bool sucessResult = false;

    if (::ReadFile(this->descriptor, (PVOID)data, (DWORD)len, &readBytes, &this->oRead))
        sucessResult = true;
    else {
        ::DWORD rc = ::GetLastError();
        if (ERROR_IO_PENDING == rc) {
            //not to loop the function, instead of setting INFINITE put 5000 milliseconds wait!
            rc = ::WaitForSingleObject(this->oRead.hEvent, 5000);
            switch (rc) {
            case WAIT_OBJECT_0:
                if (::GetOverlappedResult(this->descriptor, &this->oRead, &readBytes, false))
                    sucessResult = true;
                else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
                    qDebug("Windows: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n"
                           " -> function: ::GetOverlappedResult(this->descriptor, &this->oRead, &readBytes, false) returned: false. Error! \n");
#endif
                    ;
                }
                break;
            default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
                qDebug() << "Windows: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n"
                        " -> function: ::::WaitForSingleObject(this->oRead.hEvent, 5000) returned: " << rc << ". Error! \n";
#endif
                ;
            }
        } // ERROR_IO_PENDING
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
            qDebug() << "Windows: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n"
                    " -> function: ::GetLastError() returned: " << rc << ". Error! \n";
#endif
            ;
        }
    }

    return (sucessResult) ? qint64(readBytes) : qint64(-1);
}

int NativeSerialEnginePrivate::nativeSelect(int timeout,
                                            bool checkRead, bool checkWrite,
                                            bool *selectForRead, bool *selectForWrite)
{
    if (checkRead && (this->nativeBytesAvailable() > 0)) {
        *selectForRead = true;
        return 1;
    }

    if (!clear_overlapped(&this->oSelect))
        return int(-1);

    ::DWORD oldEventMask = 0;
    ::DWORD currEventMask = 0;

    if (checkRead)
        currEventMask |= EV_RXCHAR;
    if (checkWrite)
        currEventMask |= EV_TXEMPTY;

    //save old mask
    if(0 == ::GetCommMask(this->descriptor, &oldEventMask)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                " -> function: ::GetCommMask(this->descriptor, eMask) returned: 0. Error! \n";
#endif
        return int(-1);
    }

    if (currEventMask != (oldEventMask & currEventMask)) {
        currEventMask |= oldEventMask;
        //set mask
        if(0 == ::SetCommMask(this->descriptor, currEventMask)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
            qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                    " -> function: ::SetCommMask(this->descriptor, eMask) returned: 0. Error! \n";
#endif
            return int(-1);
        }
    }

    currEventMask = 0;
    bool selectResult = false;

    if (::WaitCommEvent(this->descriptor, &currEventMask, &this->oSelect))
        selectResult = true;
    else {
        ::DWORD rc = ::GetLastError();
        if (ERROR_IO_PENDING == rc) {
            rc = ::WaitForSingleObject(this->oSelect.hEvent, (timeout < 0) ? 0 : timeout);
            switch (rc) {
            case WAIT_OBJECT_0:
                if (::GetOverlappedResult(this->descriptor, &this->oSelect, &rc, false))
                    selectResult = true;
                else {
                    rc = ::GetLastError();
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
                    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                            " -> function: ::GetOverlappedResult(this->descriptor, &this->oSelect, &rc, false), \n"
                            " returned: false, last error: " << rc << ". Error! \n";
#endif
                    ;
                }
                break;
            case WAIT_TIMEOUT:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
                qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                        " -> function: ::WaitForSingleObject(this->oSelect.hEvent, timeout < 0 ? 0 : timeout), \n"
                        " returned: WAIT_TIMEOUT: " << rc << ". Warning! \n";
#endif
                ;
                break;
            default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
                qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                        " -> function: ::WaitForSingleObject(this->oSelect.hEvent, timeout < 0 ? 0 : timeout), \n"
                        " returned: " << rc << ". Error! \n";
#endif
                ;
            }
        }
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
            qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                    " -> function: ::GetLastError() returned: " << rc << ". Error! \n";
#endif
            ;
        }
    }

    if (selectResult) {
        *selectForRead = (currEventMask & EV_RXCHAR)
                          && (checkRead)
                          && (this->nativeBytesAvailable());
        *selectForWrite = (currEventMask & EV_TXEMPTY) && (checkWrite);
    }
    ::SetCommMask(this->descriptor, oldEventMask); //rerair old mask
    return int(selectResult);
}

//added 05.11.2009 (experimental)
qint64 NativeSerialEnginePrivate::nativeCurrentQueue(NativeSerialEngine::ioQueue Queue) const
{
    Q_UNUSED(Queue)
    /*
    COMMPROP commProp = {0};

    if (!(::GetCommProperties(this->descriptor, &commProp))) {
    #if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeCurrentQueue(NativeSerialEngine::ioQueue Queue) \n"
    " -> function: ::GetCommProperties(this->descriptor, &commProp) returned: false. Error! \n";
    #endif
    return (qint64)-1;
    }

    switch (Queue) {
    case NativeSerialEngine::txQueue : return (qint64)commProp.dwCurrentTxQueue;
    case NativeSerialEngine::rxQueue : return (qint64)commProp.dwCurrentRxQueue;
    default: return (qint64)-1;
    }
    */
    return qint64(-1);
}

// Clear all used variables.
void NativeSerialEnginePrivate::initVariables()
{
    size_t size = sizeof(::DCB);
    ::memset((void *)(&this->dcb), 0, size);
    ::memset((void *)(&this->olddcb), 0, size);
    size = sizeof(::COMMTIMEOUTS);
    ::memset((void *)(&this->ct), 0, size);
    ::memset((void *)(&this->oldct), 0, size);
    size = sizeof(::OVERLAPPED);
    ::memset((void *)(&this->oRead), 0, size);
    ::memset((void *)(&this->oWrite), 0, size);
    ::memset((void *)(&this->oSelect), 0, size);

    this->notifier = 0;
    this->descriptor = INVALID_HANDLE_VALUE;
    this->deviceName = this->defaultDeviceName;
    this->status = AbstractSerial::ENone;
    this->oldSettingsIsSaved = false;
}

/* Defines a parameter - the current baud rate of default,
   which can be obtained only after the open of the device (port).
   Used only in method: NativeSerialEnginePrivate::detectDefaultCurrentSettings().
   Always return true!!!
*/
bool NativeSerialEnginePrivate::detectDefaultBaudRate()
{
    this->ibaudRate = this->obaudRate = quint32(this->dcb.BaudRate);

#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    if (!this->dcb.BaudRate) {
        qDebug() << "Windows: NativeSerialEnginePrivate::detectDefaultBaudRate() \n"
                " -> undefined baud rate, this->cc.dcb.BaudRate is: " << this->dcb.BaudRate << " \n";
    }
#endif
    return true;
}

/* Defines a parameter - the current data bits of default,
   which can be obtained only after the open of the device (port).
   Used only in method: NativeSerialEnginePrivate::detectDefaultCurrentSettings().
   Always return true!!!
*/
bool NativeSerialEnginePrivate::detectDefaultDataBits()
{
    switch (this->dcb.ByteSize) {
    case 5: this->dataBits = AbstractSerial::DataBits5; break;
    case 6: this->dataBits = AbstractSerial::DataBits6; break;
    case 7: this->dataBits = AbstractSerial::DataBits7; break;
    case 8: this->dataBits = AbstractSerial::DataBits8; break;
    default:
        this->dataBits = AbstractSerial::DataBitsUndefined;
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::detectDefaultDataBits() \n"
                " -> undefined data bits, this->cc.dcb.ByteSize is: " << this->dcb.ByteSize << " \n";
#endif
        ;
    }
    return true;
}

/* Defines a parameter - the current parity of default,
   which can be obtained only after the open of the device (port).
   Used only in method: NativeSerialEnginePrivate::detectDefaultCurrentSettings().
   Always return true!!!
*/
bool NativeSerialEnginePrivate::detectDefaultParity()
{
    BYTE bparity = this->dcb.Parity;
    ::DWORD fparity = this->dcb.fParity;

    if ((NOPARITY == bparity) && (!fparity)) {
        this->parity = AbstractSerial::ParityNone;
        return true;
    }
    if ((SPACEPARITY == bparity) && (fparity)) {
        this->parity = AbstractSerial::ParitySpace;
        return true;
    }
    if ((MARKPARITY == bparity) && (fparity)) {
        this->parity = AbstractSerial::ParityMark;
        return true;
    }
    if ((EVENPARITY == bparity) && (fparity)) {
        this->parity = AbstractSerial::ParityEven;
        return true;
    }
    if ((ODDPARITY == bparity) && (fparity)) {
        this->parity = AbstractSerial::ParityOdd;
        return true;
    }
    this->parity = AbstractSerial::ParityUndefined;
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::detectDefaultParity() \n"
            " -> undefined parity, this->cc.dcb.Parity is: " << bparity << ", this->cc.dcb.fParity is: " << fparity << " \n";
#endif
    return true;
}

/* Defines a parameter - the current stop bits of default,
   which can be obtained only after the open of the device (port).
   Used only in method: NativeSerialEnginePrivate::detectDefaultCurrentSettings().
   Always return true!!!
*/
bool NativeSerialEnginePrivate::detectDefaultStopBits()
{
    switch (this->dcb.StopBits) {
    case ONESTOPBIT: this->stopBits = AbstractSerial::StopBits1; break;
    case ONE5STOPBITS: this->stopBits = AbstractSerial::StopBits1_5; break;
    case TWOSTOPBITS: this->stopBits = AbstractSerial::StopBits2; break;
    default:
        this->stopBits = AbstractSerial::StopBitsUndefined;
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug() << "Windows: NativeSerialEnginePrivate::detectDefaultStopBits() \n"
                " -> undefined stop bits, this->cc.dcb.StopBits is: " << this->dcb.StopBits << " \n";
#endif
        ;
    }
    return true;
}

/* Defines a parameter - the current flow control of default,
   which can be obtained only after the open of the device (port).
   Used only in method: NativeSerialEnginePrivate::detectDefaultCurrentSettings().
   Always return true!!!
*/
bool NativeSerialEnginePrivate::detectDefaultFlowControl()
{
    //TODO: here variables in future removed and replace
    ::DWORD f_OutxCtsFlow = this->dcb.fOutxCtsFlow;
    ::DWORD f_RtsControl = this->dcb.fRtsControl;
    ::DWORD f_InX = this->dcb.fInX;
    ::DWORD f_OutX = this->dcb.fOutX;

    if ((!f_OutxCtsFlow) && (RTS_CONTROL_DISABLE == f_RtsControl)
        && (!f_InX) && (!f_OutX)) {
        this->flow = AbstractSerial::FlowControlOff;
        return true;
    }
    if ((!f_OutxCtsFlow) && (RTS_CONTROL_DISABLE == f_RtsControl)
        && (f_InX) && (f_OutX)) {
        this->flow = AbstractSerial::FlowControlXonXoff;
        return true;
    }
    if ((f_OutxCtsFlow) && (RTS_CONTROL_HANDSHAKE == f_RtsControl)
        && (!f_InX) && (!f_OutX)) {
        this->flow = AbstractSerial::FlowControlHardware;
        return true;
    }
    this->flow = AbstractSerial::FlowControlUndefined;
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::detectDefaultFlowControl() \n"
            " -> undefined flow, this->cc.dcb.fOutxCtsFlow is: " << f_OutxCtsFlow
            << ", this->cc.dcb.fRtsControl is: " << f_RtsControl
            << ", this->cc.dcb.fInX is: " << f_InX
            << ", this->cc.dcb.fOutX is: " << f_OutX
            << " \n";
#endif
    return true;
}

/* Specifies the port settings that were with him by default when you open it.
   Used only in method: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode).
   Always return true!!!
*/
bool NativeSerialEnginePrivate::detectDefaultCurrentSettings()
{
    return (this->detectDefaultBaudRate() && this->detectDefaultDataBits() && this->detectDefaultParity()
             && this->detectDefaultStopBits() && this->detectDefaultFlowControl());
}

/* Prepares other parameters of the structures port configuration.
   Used only in method: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode).
*/
void NativeSerialEnginePrivate::prepareOtherOptions()
{
    this->dcb.fBinary = true;
    this->dcb.fInX = this->dcb.fOutX = this->dcb.fAbortOnError = this->dcb.fNull = false;
}

/* Used only in method:
   NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode).
*/
bool NativeSerialEnginePrivate::saveOldSettings()
{
    // Save DCB.
    ::DWORD confSize = sizeof(::DCB);
    if (0 == ::GetCommState(this->descriptor, &this->olddcb))
        return false;
    ::memcpy((void *)(&this->dcb), (const void *)(&this->olddcb), confSize);

    // Save COMMTIMEOUTS.
    confSize = sizeof(::COMMTIMEOUTS);
    if (0 == ::GetCommTimeouts(this->descriptor, &this->oldct))
        return false;
    ::memcpy((void *)(&this->ct), (const void *)(&this->oldct), confSize);

    //Set flag "altered parameters is saved"
    this->oldSettingsIsSaved = true;
    return true;
}

/* Used only in method:
   NativeSerialEnginePrivate::nativeClose().
*/
bool NativeSerialEnginePrivate::restoreOldSettings()
{
    bool restoreResult = true;
    if (this->oldSettingsIsSaved) {
        if (0 != ::GetCommState(this->descriptor, &this->olddcb))
            restoreResult = false;
        if (0 == ::SetCommTimeouts(this->descriptor, &this->oldct))
            restoreResult = false;
    }
    return restoreResult;
}

/* Used only in method:
   NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode).
*/
bool NativeSerialEnginePrivate::createEvents(bool rx, bool tx)
{
    if (rx) { this->oRead.hEvent = ::CreateEvent(0, false, false, 0); }
    if (tx) { this->oWrite.hEvent = ::CreateEvent(0, false, false, 0); }
    this->oSelect.hEvent = ::CreateEvent(0, false, false, 0);

    return ((rx && (0 == this->oRead.hEvent))
            || (tx && (0 == this->oWrite.hEvent))
            || (0 == this->oSelect.hEvent)) ? false : true;
}

/* Used only in method:
   NativeSerialEnginePrivate::nativeClose().
*/
bool NativeSerialEnginePrivate::closeEvents() const
{
    bool closeResult = true;
    if ((this->oRead.hEvent) && (0 == ::CloseHandle(this->oRead.hEvent)))
        closeResult = false;
    if ((this->oWrite.hEvent) && (0 == ::CloseHandle(this->oWrite.hEvent)))
        closeResult = false;
    if ((this->oSelect.hEvent) && (0 == ::CloseHandle(this->oSelect.hEvent)))
        closeResult = false;

    return closeResult;
}

/* This method automatically calculates the time-out reading of the package,
   taking into account the speed, of number of data bits, stop bits and parity bit.
*/
void NativeSerialEnginePrivate::recalcTotalReadTimeoutConstant()
{
    if (this->isAutoCalcReadTimeoutConstant) {
        quint32 currReadTimeoutConstant = quint32(this->ct.ReadTotalTimeoutConstant);
        // Here calculate length one symbol, in bits (count bits in symbol).
        quint32 frameBitsLen = 1; //add start bit
        frameBitsLen += this->currDataBitsToValue(); //add data bits
        frameBitsLen += this->currParityToValue(); //add parity bit
        frameBitsLen += this->currStopBitsToValue(); //add stop bits
        // Here calculate length all symbol in packet, in bits.
        frameBitsLen *= READ_CHUNKSIZE;
        //
        if ((this->ibaudRate != this->obaudRate) || (ibaudRate <= 0))
            return;

        // Without an interval of silence between the characters, in msecs
        quint32 calcReadTimeoutConstant = (frameBitsLen * 1000) / this->ibaudRate;
        // Add interval of silence between the characters.
        calcReadTimeoutConstant += ((READ_CHUNKSIZE - 1) * this->charIntervalTimeout) / 1000;

        if (currReadTimeoutConstant != calcReadTimeoutConstant) {
            this->prepareCommTimeouts(NativeSerialEnginePrivate::ReadTotalTimeoutConstant,
                                      ::DWORD(calcReadTimeoutConstant));
            this->updateCommTimeouts();
        }
    }
}

void NativeSerialEnginePrivate::prepareCommTimeouts(CommTimeouts cto, ::DWORD msecs)
{
    switch (cto) {
    case NativeSerialEnginePrivate::ReadIntervalTimeout:
        if (this->ct.ReadIntervalTimeout != msecs)
            this->ct.ReadIntervalTimeout = msecs;
        break;
    case NativeSerialEnginePrivate::ReadTotalTimeoutMultiplier:
        if (this->ct.ReadTotalTimeoutMultiplier != msecs)
            this->ct.ReadTotalTimeoutMultiplier = msecs;
        break;
    case NativeSerialEnginePrivate::ReadTotalTimeoutConstant:
        if (this->ct.ReadTotalTimeoutConstant != msecs)
            this->ct.ReadTotalTimeoutConstant = msecs;
        break;
    case NativeSerialEnginePrivate::WriteTotalTimeoutMultiplier:
        if (this->ct.WriteTotalTimeoutMultiplier != msecs)
            this->ct.WriteTotalTimeoutMultiplier = msecs;
        break;
    case NativeSerialEnginePrivate::WriteTotalTimeoutConstant:
        if (this->ct.WriteTotalTimeoutConstant != msecs)
            this->ct.WriteTotalTimeoutConstant = msecs;
        break;
    default:;
    }
}

bool NativeSerialEnginePrivate::updateDcb()
{
    return (0 != ::SetCommState(this->descriptor, &this->dcb));
}

bool NativeSerialEnginePrivate::updateCommTimeouts()
{
    return (0 != ::SetCommTimeouts(this->descriptor, &this->ct));
}
