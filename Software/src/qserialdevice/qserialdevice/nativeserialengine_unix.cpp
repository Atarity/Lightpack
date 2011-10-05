/*
* This file is part of QSerialDevice, an open-source cross-platform library
* Copyright (C) 2009  Denis Shienkov
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundathis->tion; either version 2 of the License, or
* (at your opthis->tion) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundathis->tion, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact Denis Shienkov:
*          e-mail: <scapig2@yandex.ru>
*             ICQ: 321789831
*/

#include <QtCore/QDir>

#include "../unix/qcore_unix_p.h"

#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#if defined (Q_OS_LINUX)
  #include <linux/serial.h>
#endif

#include "nativeserialengine_p.h"

//#define NATIVESERIALENGINE_UNIX_DEBUG

#ifdef NATIVESERIALENGINE_UNIX_DEBUG
#include <QtCore/QDebug>
#endif


NativeSerialEnginePrivate::NativeSerialEnginePrivate()
{
    this->initVariables();
}

bool NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode)
{
    //here chek locked device or not
    this->locker.setDeviceName(this->deviceName);
    bool byCurrPid = false;
    if (this->locker.locked(&byCurrPid)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> can not open the device:" << this->deviceName <<
                "\n, because it was locked. Error! \n";
#endif
        return false;
    }

    int flags = (O_NOCTTY | O_NDELAY);
    switch (QIODevice::ReadWrite & mode) {
    case QIODevice::ReadOnly: flags |= (O_RDONLY); break;
    case QIODevice::WriteOnly: flags |= (O_WRONLY); break;
    case QIODevice::ReadWrite: flags |= (O_RDWR); break;
    default:;
    }

    //try opened serial device
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
            " -> trying to open device: " << this->deviceName << " \n";
#endif

    this->descriptor = qt_safe_open(this->deviceName.toLocal8Bit().constData(), flags);

    if (-1 == this->descriptor) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: ::open(...) returned: -1,"
                "errno:" << errno << ". Error! \n";
#endif
        return false;
    }

    //here try lock device
    this->locker.lock();
    if (!this->locker.locked(&byCurrPid)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> an error occurred when locking the device:" << this->deviceName << ". Error! \n";
#endif
        return false;
    }

    //here try set exclusive mode
#if defined (TIOCEXCL)
    ::ioctl(this->descriptor, TIOCEXCL);
#endif

    if (!this->saveOldSettings()) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: saveOldSettings() returned: false. Error! \n";
#endif
        return false;
    }

    //Prepare other options
    this->prepareOtherOptions();
    this->prepareTimeouts(0);

    if (!this->updateTermious()) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: ::updateTermious() returned: false. Error! \n";
#endif
        return false;
    }

    // Disable autocalculate total read interval.
    this->isAutoCalcReadTimeoutConstant = false;

    if (!this->detectDefaultCurrentSettings()) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
                " -> function: detectDefaultCurrentSettings() returned: false. Error! \n";
#endif
        return false;
    }

#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode) \n"
            " -> opened device: " << this->deviceName << " in mode: " << mode << " succesfully. Ok! \n";
#endif

    return true;
}

bool NativeSerialEnginePrivate::nativeClose()
{
    //here try clean exclusive mode
#if defined (TIOCNXCL)
    ::ioctl(this->descriptor, TIOCNXCL);
#endif

    bool closeResult = true;

    if (-1 == this->descriptor)
        return closeResult;

    //try restore old parameters device
    if (!this->restoreOldSettings()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeClose() \n"
               " -> function: restoreOldSettings() returned: false. Error! \n");
#endif
        //closeResult = false;
    }

    //try closed device
    if (-1 == qt_safe_close(this->descriptor)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeClose() \n"
               " -> function: ::close(this->descriptor) returned: -1. Error! \n");
#endif
        //closeResult = false;
    }

    //here try unlock device
    this->locker.setDeviceName(this->deviceName);
    bool byCurrPid = false;

    if (this->locker.locked(&byCurrPid) && byCurrPid)
        this->locker.unlock();

    //if (closeResult) {
        this->descriptor = -1;
    //}

    return closeResult;
}

/*
    Attempts to convert the speed in the POSIX constant speed.
    If the system does not have the necessary constants, the function returns 0.
*/
static speed_t value_baud_to_nix_type_baud(qint32 baudRate)
{
    switch (baudRate) {
    case 50:
#if defined (B50)
        return B50;
#endif
        break;
    case 75:
#if defined (B75)
        return B75;
#endif
        break;
    case 110:
#if defined (B110)
        return B110;
#endif
        break;
    case 134:
#if defined (B134)
        return B134;
#endif
        break;
    case 150:
#if defined (B150)
        return B150;
#endif
        break;
    case 200:
#if defined (B50)
        return B50;
#endif
        break;
    case 300:
#if defined (B300)
        return B300;
#endif
        break;
    case 600:
#if defined (B600)
        return B600;
#endif
        break;
    case 1200:
#if defined (B1200)
        return B1200;
#endif
        break;
    case 1800:
#if defined (B1800)
        return B1800;
#endif
        break;
    case 2400:
#if defined (B2400)
        return B2400;
#endif
        break;
    case 4800:
#if defined (B4800)
        return B4800;
#endif
        break;
    case 9600:
#if defined (B9600)
        return B9600;
#endif
        break;
    case 19200:
#if defined (B19200)
        return B19200;
#endif
        break;
    case 38400:
#if defined (B38400)
        return B38400;
#endif
        break;
    case 57600:
#if defined (B57600)
        return B57600;
#endif
        break;
    case 115200:
#if defined (B115200)
        return B115200;
#endif
        break;
    case 230400:
#if defined (B230400)
        return B230400;
#endif
        break;
    case 460800:
#if defined (B460800)
        return B460800;
#endif
        break;
    case 500000:
#if defined (B500000)
        return B500000;
#endif
        break;
    case 576000:
#if defined (B576000)
        return B576000;
#endif
        break;
    case 921600:
#if defined (B921600)
        return B921600;
#endif
        break;
    case 1000000:
#if defined (B1000000)
        return B1000000;
#endif
        break;
    case 1152000:
#if defined (B1152000)
        return B1152000;
#endif
        break;
    case 1500000:
#if defined (B1500000)
        return B1500000;
#endif
        break;
    case 2000000:
#if defined (B2000000)
        return B2000000;
#endif
        break;
    case 2500000:
#if defined (B2500000)
        return B50;
#endif
        break;
    case 3000000:
#if defined (B3000000)
        return B3000000;
#endif
        break;
    case 3500000:
#if defined (B3500000)
        return B3500000;
#endif
        break;
    case 4000000:
#if defined (B4000000)
        return B4000000;
#endif
        break;
    default:;
    }
    return 0;
}

bool NativeSerialEnginePrivate::nativeSetBaudRate(qint32 baudRate, AbstractSerial::BaudRateDirection baudDir)
{
    speed_t speed = value_baud_to_nix_type_baud(baudRate);
    if (speed > 0) {
        if (this->setStandartBaud(baudDir, speed)) {
            if (AbstractSerial::InputBaud & baudDir)
                this->ibaudRate = baudRate;
            if (AbstractSerial::OutputBaud & baudDir)
                this->obaudRate = baudRate;
            //this->recalcReadTimeout();
            return true;
        }
    }
    else {
        if (this->setCustomBaud(baudRate)) {
            this->ibaudRate = this->obaudRate = baudRate;
            //this->recalcReadTimeout();
            return true;
        }
    }
    return false;
}

bool NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits)
{
    if ((AbstractSerial::DataBits5 == dataBits)
        && (AbstractSerial::StopBits2 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
               " -> 5 data bits cannot be used with 2 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits6 == dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
               " -> 6 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits7 == dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
               " -> 7 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    if ((AbstractSerial::DataBits8 == dataBits)
        && (AbstractSerial::StopBits1_5 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
               " -> 8 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

#if !defined (CMSPAR)
    /* This protection against the installation of parity in the absence of macro Space CMSPAR.
    (That is prohibited for any *.nix than GNU/Linux) */

    if ((AbstractSerial::ParitySpace == this->parity)
        && (AbstractSerial::DataBits8 == dataBits)) {

#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> parity: " << this->parity << " is not supported by the class NativeSerialEnginePrivate, \n"
                "space parity is only supported in POSIX with 7 or fewer data bits. Error! \n";
#endif
        return false;
    }
#endif

    switch (dataBits) {
    case AbstractSerial::DataBits5:
        this->tio.c_cflag &= (~CSIZE);
        this->tio.c_cflag |= CS5;
        break;
    case AbstractSerial::DataBits6:
        this->tio.c_cflag &= (~CSIZE);
        this->tio.c_cflag |= CS6;
        break;
    case AbstractSerial::DataBits7:
        this->tio.c_cflag &= (~CSIZE);
        this->tio.c_cflag |= CS7;
        break;
    case AbstractSerial::DataBits8:
        this->tio.c_cflag &= (~CSIZE);
        this->tio.c_cflag |= CS8;
        break;
    default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
                " -> dataBits: " << dataBits << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::DataBits. Error! \n";
#endif
        return false;
    }

    if (!this->updateTermious()) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
                " -> function: updateTermious() returned: false. Error! \n";
#endif
        return false;
    }
    this->dataBits = dataBits;
    //this->recalcReadTimeout();
    return true;
}

bool NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity)
{
    switch (parity) {
#if defined (CMSPAR) //CMSPAR
    // Here Installation parity only for GNU/Linux where the macro CMSPAR.
    case AbstractSerial::ParitySpace:
        this->tio.c_cflag &= (~PARODD);
        this->tio.c_cflag |= (PARENB | CMSPAR);
        break;
    case AbstractSerial::ParityMark:
        this->tio.c_cflag |= (PARENB | CMSPAR | PARODD);
        break;
#else //CMSPAR
    // Here setting parity to other *.nix.
    // TODO: check here impl!
    case AbstractSerial::ParitySpace:
        switch (this->dataBits) {
        case AbstractSerial::DataBits5:
            this->tio.c_cflag &= ~(PARENB | CSIZE);
            this->tio.c_cflag |= CS6;
            break;
        case AbstractSerial::DataBits6:
            this->tio.c_cflag &= ~(PARENB | CSIZE);
            this->tio.c_cflag |= CS7;
            break;
        case AbstractSerial::DataBits7:
            this->tio.c_cflag &= ~(PARENB | CSIZE);
            this->tio.c_cflag |= CS8;
            break;
        case AbstractSerial::DataBits8:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
            qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                    " -> parity: " << parity << " is not supported by the class NativeSerialEnginePrivate, \n"
                    "space parity is only supported in POSIX with 7 or fewer data bits. Error! \n";
#endif
            return false;
        default: ;
        }
        break;
    case AbstractSerial::ParityMark:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> parity: " << parity << " is not supported by the class NativeSerialEnginePrivate, \n"
                "mark parity is not supported by POSIX.. Error! \n";
#endif
        return false;
#endif //CMSPAR

    case AbstractSerial::ParityNone:
        this->tio.c_cflag &= (~PARENB);
        break;
    case AbstractSerial::ParityEven:
        this->tio.c_cflag &= (~PARODD);
        this->tio.c_cflag |= PARENB;
        break;
    case AbstractSerial::ParityOdd:
        this->tio.c_cflag |= (PARENB | PARODD);
        break;
    default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> parity: " << parity << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::Parity. Error! \n";
#endif
        return false;
    }

    if (!this->updateTermious()) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> function: updateTermious() returned: false. Error! \n";
#endif
        return false;
    }
    this->parity = parity;
    //this->recalcReadTimeout();
    return true;
}

bool NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits)
{
    if ((AbstractSerial::DataBits5 == this->dataBits)
        && (AbstractSerial::StopBits2 == this->stopBits)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
               " -> 5 data bits cannot be used with 2 stop bits. Error! \n");
#endif
        return false;
    }

    switch (stopBits) {
    case AbstractSerial::StopBits1:
        this->tio.c_cflag &= (~CSTOPB);
        break;
    case AbstractSerial::StopBits2:
        this->tio.c_cflag |= CSTOPB;
        break;
    case AbstractSerial::StopBits1_5:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
               " -> 1.5 stop bit operathis->tion is not supported by POSIX. Error! \n");
#endif
        return false;

    default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
                " -> stopBits: " << stopBits << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::StopBits. Error! \n";
#endif
        return false;
    }//switch

    if (-1 == ::tcsetattr(this->descriptor, TCSANOW, &this->tio)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
                " -> function: ::tcsetattr(this->descriptor, TCSANOW, &this->tio) returned: -1. Error! \n";
#endif
        return false;
    }
    this->stopBits = stopBits;
    //this->recalcReadTimeout();
    return true;
}

bool NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow)
{
    switch (flow) {
    case AbstractSerial::FlowControlOff:
        this->tio.c_cflag &= (~CRTSCTS);
        this->tio.c_iflag &= (~(IXON | IXOFF | IXANY));
        break;
    case AbstractSerial::FlowControlXonXoff:
        this->tio.c_cflag &= (~CRTSCTS);
        this->tio.c_iflag |= (IXON | IXOFF | IXANY);
        break;
    case AbstractSerial::FlowControlHardware:
        this->tio.c_cflag |= CRTSCTS;
        this->tio.c_iflag &= (~(IXON | IXOFF | IXANY));
        break;
    default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
                " -> flow: " << flow << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::Flow. Error! \n";
#endif
        return false;
    }//switch

    if (!this->updateTermious()) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
                " -> function: updateTermious() returned: false. Error! \n";
#endif
        return false;
    }
    this->flow = flow;
    return true;
}

void NativeSerialEnginePrivate::nativeSetCharReadTimeout(int usecs)
{
    this->charIntervalTimeout = usecs;
}

int NativeSerialEnginePrivate::nativeCharReadTimeout() const
{
    return this->charIntervalTimeout;
}

void NativeSerialEnginePrivate::nativeSetTotalReadConstantTimeout(int msecs)
{
    Q_UNUSED(msecs)
    // Empty.
}

int NativeSerialEnginePrivate::nativeTotalReadConstantTimeout() const
{
    // Not support in *.nix.
    return quint32(0);
}

bool NativeSerialEnginePrivate::nativeSetDtr(bool set) const
{
    int status = 0;

    if (-1 == ::ioctl(this->descriptor, TIOCMGET, &status)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetDtr(bool set) \n"
               " -> function: ::ioctl(this->descriptor, TIOCMGET, &status) returned: -1. Error! \n");
#endif
        return false;
    }

    (set) ? status |= TIOCM_DTR : status &= (~TIOCM_DTR);

    if (-1 == ::ioctl(this->descriptor, TIOCMSET, &status)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetDtr(bool set) \n"
               " -> function: ::ioctl(this->descriptor, TIOCMSET, &status) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeSetRts(bool set) const
{
    int status = 0;

    if (-1 == ::ioctl(this->descriptor, TIOCMGET, &status)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
               " -> function: ::ioctl(this->descriptor, TIOCMGET, &status) returned: -1. Error! \n");
#endif
        return false;
    }

    (set) ? status |= TIOCM_RTS : status &= (~TIOCM_RTS);

    if (-1 == ::ioctl(this->descriptor, TIOCMSET, &status)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
               " -> function: ::ioctl(this->descriptor, TIOCMSET, &status) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

quint16 NativeSerialEnginePrivate::nativeLineStatus() const
{
    int temp = 0;
    quint16 status = AbstractSerial::LineErr;

    if (-1 == ::ioctl(this->descriptor, TIOCMGET, &temp)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeLineStatus() \n"
               " -> function: ::ioctl(this->descriptor, TIOCMGET, &temp) returned: -1. Error! \n");
#endif
        return status;
    }

    status &= (~status);
#if defined (TIOCM_LE)
    if (temp & TIOCM_LE) status |= AbstractSerial::LineLE;
#endif
#if defined (TIOCM_DTR)
    if (temp & TIOCM_DTR) status |= AbstractSerial::LineDTR;
#endif
#if defined (TIOCM_RTS)
    if (temp & TIOCM_RTS) status |= AbstractSerial::LineRTS;
#endif
#if defined (TIOCM_ST)
    if (temp & TIOCM_ST) status |= AbstractSerial::LineST;
#endif
#if defined (TIOCM_SR)
    if (temp & TIOCM_SR) status |= AbstractSerial::LineSR;
#endif
#if defined (TIOCM_CTS)
    if (temp & TIOCM_CTS) status |= AbstractSerial::LineCTS;
#endif

#if defined (TIOCM_CAR)
    if (temp & TIOCM_CAR) status |= AbstractSerial::LineDCD;
#elif defined (TIOCM_CD)
    if (temp & TIOCM_CD) status |= AbstractSerial::LineDCD;
#endif

#if defined (TIOCM_RNG)
    if (temp & TIOCM_RNG) status |= AbstractSerial::LineRI;
#elif defined (TIOCM_RI)
    if (temp & TIOCM_RI) status |= AbstractSerial::LineRI;
#endif

#if defined (TIOCM_DSR)
    if (temp & TIOCM_DSR) status |= AbstractSerial::LineDSR;
#endif

    return status;
}

bool NativeSerialEnginePrivate::nativeSendBreak(int duration) const
{
    if (-1 == ::tcsendbreak(this->descriptor, duration)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
               " -> function: ::tcsendbreak(this->descriptor, duration) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeSetBreak(bool set) const
{
    if (set) {
        if (-1 == ::ioctl(this->descriptor, TIOCSBRK)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
            qDebug("Linux: NativeSerialEnginePrivate::nativeSetBreak(bool set) \n"
                   " -> function: ::ioctl(this->descriptor, TIOCSBRK) returned: -1. Error! \n");
#endif
            return false;
        }
        return true;
    }

    if (-1 == ::ioctl(this->descriptor, TIOCCBRK)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeSetBreak(bool set) \n"
               " -> function: ::ioctl(this->descriptor), TIOCCBRK) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeFlush() const
{
    if (-1 == ::tcdrain(this->descriptor)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeFlush() \n"
               " -> function: ::tcdrain(this->descriptor) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeReset() const
{
    if (-1 == ::tcflush(this->descriptor, TCIOFLUSH)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug("Linux: NativeSerialEnginePrivate::nativeReset() \n"
               " -> function: ::tcflush(this->descriptor, TCIOFLUSH) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

qint64 NativeSerialEnginePrivate::nativeBytesAvailable() const
{
    qint64 nbytes = 0;
    //TODO: whether there has been done to build a multi-platform *.nix. (FIONREAD) ?
    int arg = 0;
#if defined (FIONREAD)
    arg = FIONREAD;
#else
    arg = TIOCINQ;
#endif
    if (-1 == ::ioctl(this->descriptor, arg, &nbytes)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
            qDebug("Linux: NativeSerialEnginePrivate::nativeBytesAvailable(bool wait) \n"
                   " -> function: ::ioctl(this->descriptor, FIONREAD, &nbytes) returned: -1. Error! \n");
#endif
        nbytes = -1;
    }
    return nbytes;
}

qint64 NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    qint64 bytesWritten = qt_safe_write(this->descriptor, (const void *)data, (size_t)len);

    this->nativeFlush();

    if (bytesWritten < 0) {
        switch (errno) {
        case EPIPE:
        case ECONNRESET:
            bytesWritten = -1;
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
            qDebug() << "Linux: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) write fail: \n"
                    " - bytes written, bytesWritten: " << bytesWritten << " bytes \n"
                    " - error code,           errno: " << errno << " \n. Error! \n";
#endif
            this->nativeClose();
            break;
        case EAGAIN:
            bytesWritten = 0;
            break;
        case EMSGSIZE:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
            qDebug() << "Linux: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) write fail: \n"
                    " - bytes written, bytesWritten: " << bytesWritten << " bytes \n"
                    " - error code,           errno: " << errno << " \n. Error! \n";
#endif
            break;
        default:;
        }
    }

    return bytesWritten;
}

//FIXME
qint64 NativeSerialEnginePrivate::nativeRead(char *data, qint64 len)
{
    /*
        WARNING!!!

        Here, I was not able to implement blocking read-through setting termios
        (see explanation in prepareTimeouts()).

        Need to implement a reading with this algorithm:

        1. If the timeout interval of the symbol is greater than 0, then waiting to read this character.
        2. If the timeout interval of the symbol 0, then return the reading immediately!

        So I had to use ::select(). Other solution I have found.

    */
    qint64 bytesReaded = 0;

    QTime readDelay;
    readDelay.start();

    bool sfr = false;
    bool sfw = false;

    int msecs = this->charIntervalTimeout / 1000;

    do {
        qint64 readFromDevice = qt_safe_read(this->descriptor, (void*)data, len - bytesReaded);
        if (readFromDevice < 0) {
            bytesReaded = readFromDevice;
            break;
        }
        bytesReaded += readFromDevice;

    } while ((msecs > 0) && (this->nativeSelect(msecs, true, false, &sfr, &sfw) > 0) && (bytesReaded < len));


    if (bytesReaded < 0) {
        bytesReaded = -1;
        switch (errno) {
#if EWOULDBLOCK-0 && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
#endif
        case EAGAIN:
            // No data was available for reading
            bytesReaded = -2;
            break;
        case EBADF:
        case EINVAL:
        case EIO:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n -> read fail: \n"
                " - bytes readed, bytesReaded: " << bytesReaded << " bytes \n"
                " - error code,         errno: " << errno << " \n. Error! \n";
#endif
            break;
#ifdef Q_OS_SYMBIAN
        case EPIPE:
#endif
        case ECONNRESET:
#if defined (Q_OS_VXWORKS)
        case ESHUTDOWN:
#endif
            bytesReaded = 0;
            break;
        default:;
        }
    }

    return bytesReaded;
}

int NativeSerialEnginePrivate::nativeSelect(int timeout,
                                            bool checkRead, bool checkWrite,
                                            bool *selectForRead, bool *selectForWrite)
{
    fd_set fdread;
    FD_ZERO(&fdread);
    if (checkRead)
        FD_SET(this->descriptor, &fdread);

    fd_set fdwrite;
    FD_ZERO(&fdwrite);
    if (checkWrite)
        FD_SET(this->descriptor, &fdwrite);

    struct timeval tv;
    tv.tv_sec = (timeout / 1000);
    tv.tv_usec = (timeout % 1000) * 1000;

    //qt_safe_* added in Qt version >= 4.6.0
    int ret = qt_safe_select(this->descriptor + 1, &fdread, &fdwrite, 0, (timeout < 0) ? 0 : &tv);
    //int ret = ::select(this->descriptor + 1, &fdread, 0, 0, timeout < 0 ? 0 : &tv);

    if (ret <= 0) {
        *selectForRead = *selectForWrite = false;
        return ret;
    }

    *selectForRead = FD_ISSET(this->descriptor, &fdread);
    *selectForWrite = FD_ISSET(this->descriptor, &fdwrite);

    return ret;
}

//added 06.11.2009 (while is not used)
qint64 NativeSerialEnginePrivate::nativeCurrentQueue(NativeSerialEngine::ioQueue Queue) const
{
    Q_UNUSED(Queue)
    /* not supported */
    return qint64(-1);
}

void NativeSerialEnginePrivate::initVariables()
{
    ::memset((void *)(&this->tio), 0, sizeof(this->tio));
    ::memset((void *)(&oldtio), 0, sizeof(oldtio));

    this->notifier = 0;
    this->descriptor = -1;
    this->deviceName = this->defaultDeviceName;
    //
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
    //TODO: ??
    for (int i = 0; i < 2; ++i) {
        qint32 *pspeed =  0;
        speed_t result = 0;
        switch (i) {
        case 0:
            pspeed = &this->ibaudRate;
            result = ::cfgetispeed(&this->tio);
            break;
        case 1:
            pspeed = &this->obaudRate;
            result = ::cfgetospeed(&this->tio);
            break;
        default: result = 0;
        }

        switch (result) {
#if defined (B50)
        case B50: *pspeed = 50; break;
#endif
#if defined (B75)
        case B75: *pspeed = 75; break;
#endif
#if defined (B110)
        case B110: *pspeed = 110; break;
#endif
#if defined (B134)
        case B134: *pspeed = 134; break;
#endif
#if defined (B150)
        case B150: *pspeed = 150; break;
#endif
#if defined (B200)
        case B200: *pspeed = 200; break;
#endif
#if defined (B300)
        case B300: *pspeed = 300; break;
#endif
#if defined (B600)
        case B600: *pspeed = 600; break;
#endif
#if defined (B1200)
        case B1200: *pspeed = 1200; break;
#endif
#if defined (B1800)
        case B1800: *pspeed = 1800; break;
#endif
#if defined (B2400)
        case B2400: *pspeed = 2400; break;
#endif
#if defined (B4800)
        case B4800: *pspeed = 4800; break;
#endif
#if defined (B9600)
        case B9600: *pspeed = 9600; break;
#endif
#if defined (B19200)
        case B19200: *pspeed = 19200; break;
#endif
#if defined (B38400)
        case B38400: *pspeed = 38400; break;
#endif
#if defined (B57600)
        case B57600: *pspeed = 57600; break;
#endif
#if defined (B115200)
        case B115200: *pspeed = 115200;  break;
#endif
#if defined (B230400)
        case B230400: *pspeed = 230400;  break;
#endif
#if defined (B460800)
        case B460800: *pspeed = 460800;  break;
#endif
#if defined (B500000)
        case B500000: *pspeed = 500000;  break;
#endif
#if defined (B576000)
        case B576000: *pspeed = 576000;  break;
#endif
#if defined (B921600)
        case B921600: *pspeed = 921600;  break;
#endif
#if defined (B1000000)
        case B1000000: *pspeed = 1000000;  break;
#endif
#if defined (B1152000)
        case B1152000: *pspeed = 1152000;  break;
#endif
#if defined (B1500000)
        case B1500000: *pspeed = 1500000;  break;
#endif
#if defined (B2000000)
        case B2000000: *pspeed = 2000000;  break;
#endif
#if defined (B2500000)
        case B2500000: *pspeed = 2500000;  break;
#endif
#if defined (B3000000)
        case B3000000: *pspeed = 3000000;  break;
#endif
#if defined (B3500000)
        case B3500000: *pspeed = 3500000;  break;
#endif
#if defined (B4000000)
        case B4000000: *pspeed = 4000000;  break;
#endif
        default:
            *pspeed = -1;
        }
        pspeed = 0;
    }
    return true;
}

/* Defines a parameter - the current data bits of default,
   which can be obtained only after the open of the device (port).
   Used only in method: NativeSerialEnginePrivate::detectDefaultCurrentSettings().
   Always return true!!!
*/
bool NativeSerialEnginePrivate::detectDefaultDataBits()
{
    tcflag_t result = this->tio.c_cflag;

    switch (CSIZE & result) {
    case CS5:
        this->dataBits = AbstractSerial::DataBits5; break;
    case CS6:
        this->dataBits = AbstractSerial::DataBits6; break;
    case CS7:
        this->dataBits = AbstractSerial::DataBits7; break;
    case CS8:
        this->dataBits = AbstractSerial::DataBits8; break;
    default:
        this->dataBits = AbstractSerial::DataBitsUndefined;
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::detectDefaultDataBits() \n"
                " -> data bits undefined, c_cflag is: " << result << " \n";
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
    tcflag_t result = this->tio.c_cflag;

#if defined (CMSPAR)
    /* TODO: while impl only if defined CMSPAR !!!
    */
    if ((result & CMSPAR)
        && (!(result & PARODD))) {
        this->parity = AbstractSerial::ParitySpace;
        return true;
    }

    if ((result & CMSPAR)
        && (result & PARODD)) {
        this->parity = AbstractSerial::ParityMark;
        return true;
    }
#endif

    if (!(result & PARENB)) {
        this->parity = AbstractSerial::ParityNone;
        return true;
    }

    if ((result & PARENB)
        && (!(result & PARODD))) {
        this->parity = AbstractSerial::ParityEven;
        return true;
    }

    if ((result & PARENB)
        && (result & PARODD)) {
        this->parity = AbstractSerial::ParityOdd;
        return true;
    }

    this->parity = AbstractSerial::ParityUndefined;
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::detectDefaultParity() \n"
            " -> parity undefined, c_cflag is: " << result << "\n";
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
    tcflag_t result = this->tio.c_cflag;

    if (!(result & CSTOPB)) {
        this->stopBits = AbstractSerial::StopBits1;
        return true;
    }

    if (result & CSTOPB) {
        this->stopBits = AbstractSerial::StopBits2;
        return true;
    }

    this->stopBits = AbstractSerial::StopBitsUndefined;
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::detectDefaultStopBits() \n"
            " -> stop bits undefined, c_cflag is: " << result << " \n";
#endif
    return true;
}

/* Defines a parameter - the current flow control of default,
   which can be obtained only after the open of the device (port).
   Used only in method: NativeSerialEnginePrivate::detectDefaultCurrentSettings().
   Always return true!!!
*/
bool NativeSerialEnginePrivate::detectDefaultFlowControl()
{
    if ((!(this->tio.c_cflag & CRTSCTS))
        && (!(this->tio.c_iflag & (IXON | IXOFF | IXANY)))) {
        this->flow = AbstractSerial::FlowControlOff;
        return true;
    }

    if ((!(this->tio.c_cflag & CRTSCTS))
        && (this->tio.c_iflag & (IXON | IXOFF | IXANY))) {
        this->flow = AbstractSerial::FlowControlXonXoff;
        return true;
    }

    if ((this->tio.c_cflag & CRTSCTS)
        && (!(this->tio.c_iflag & (IXON | IXOFF | IXANY)))) {
        this->flow = AbstractSerial::FlowControlHardware;
        return true;
    }

    this->flow = AbstractSerial::FlowControlUndefined;
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::detectDefaultFlowControl() \n"
            " -> flow control undefined \n";
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

/* Prepares other parameters of the structures port configurathis->tion.
   Used only in method: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode)
*/
void NativeSerialEnginePrivate::prepareOtherOptions()
{
    //TODO: It is necessary to check work in Mac OSX, and if you need to make changes!
    ::cfmakeraw(&this->tio);
    //control modes [c_cflag]
    this->tio.c_cflag |= (CREAD | CLOCAL);
    //local modes [c_lflag]
    //input modes [c_iflag]
    //output modes [c_oflag]
}

/* Used only in method: NativeSerialEnginePrivate::nativeOpen(AbstractSerial::OpenMode mode)
*/
bool NativeSerialEnginePrivate::saveOldSettings()
{
    // Saved current settings to oldtio.
    if (-1 == ::tcgetattr(this->descriptor, &this->oldtio))
        return false;
    // Get configure.
    ::memcpy((void *)(&this->tio), (const void *)(&this->oldtio), sizeof(this->oldtio));
    //::memset((void *)(&this->tio), 0, sizeof(this->tio));
    // Set flag "altered parameters is saved".
    this->oldSettingsIsSaved = true;
    return true;
}

/* Used only in method: NativeSerialEnginePrivate::nativeClose()
*/
bool NativeSerialEnginePrivate::restoreOldSettings()
{
    return (this->oldSettingsIsSaved
            && (-1 == ::tcsetattr(this->descriptor, TCSANOW, &this->oldtio))) ?
            false : true;
}

//FIXME
void NativeSerialEnginePrivate::prepareTimeouts(int msecs)
{
    Q_UNUSED(msecs)

    // Only non-blocking!
    if (this->tio.c_cc[VMIN])
        this->tio.c_cc[VMIN] = 0;
    if (this->tio.c_cc[VTIME])
        this->tio.c_cc[VTIME] = 0;

    //if msecs then set ::fcntl(this->descriptor, F_SETFL, 0)

    /*
        WARNING!!!

        In my ArchLinux does not work blocking read mode.
        Installing VTIME > 0 (at ::fcntl(this->descriptor, F_SETFL, 0)) does not work,
        reading function returns immediately.
        So I gave up a blocking read!
        But to wait for a character I had to modify the method nativeRead() through ::select() (see nativeRead()).
    */

}

bool NativeSerialEnginePrivate::updateTermious()
{
    return (-1 != ::tcsetattr(this->descriptor, TCSANOW, &this->tio));
}

bool NativeSerialEnginePrivate::setStandartBaud(AbstractSerial::BaudRateDirection direction, speed_t speed)
{
    int ret = 0;
    switch (direction) {
    case AbstractSerial::InputBaud: ret = ::cfsetispeed(&this->tio, speed); break;
    case AbstractSerial::OutputBaud: ret = ::cfsetospeed(&this->tio, speed); break;
    case AbstractSerial::AllBaud: ret = ::cfsetspeed(&this->tio, speed); break;
    default: return false;
    }
    if (-1 == ret) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::setStandartBaud(int direction, speed_t speed) \n"
                " -> function: ::cfset(i/o)speed(...) returned: -1. Error! \n";
#endif
        return false;
    }
    if (!this->updateTermious()) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
        qDebug() << "Linux: NativeSerialEnginePrivate::setStandartBaud(int direction, speed_t speed) \n"
                " -> function: updateTermious() returned: false. Error! \n";
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::setCustomBaud(qint32 speed)
{
    int result = -1;
#if defined (TIOCGSERIAL) && defined (TIOCSSERIAL)
#if defined (Q_OS_LINUX)
    if (speed > 0) {
        struct serial_struct ser_info;
        result = ::ioctl(this->descriptor, TIOCGSERIAL, &ser_info);
        if (-1 != result) {
            ser_info.flags &= ~ASYNC_SPD_MASK;
            ser_info.flags |= (ASYNC_SPD_CUST /* | ASYNC_LOW_LATENCY*/);
            ser_info.custom_divisor = ser_info.baud_base / speed;
            if (ser_info.custom_divisor)
                result = ::ioctl(this->descriptor, TIOCSSERIAL, &ser_info);
        }
    }
#endif //Q_OS_LINUX
#endif
    return (-1 != result);
}
