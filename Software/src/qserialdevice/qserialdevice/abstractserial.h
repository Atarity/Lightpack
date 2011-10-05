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

#ifndef ABSTRACTSERIAL_H
#define ABSTRACTSERIAL_H

#include <QtCore/QIODevice>
#include <QtCore/QMap>
#include <QtCore/QDateTime>

#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/QDebug>
#endif

#include "../qserialdevice_global.h"

#ifndef SERIALDEVICE_BUFFERSIZE
#define SERIALDEVICE_BUFFERSIZE Q_INT64_C(16384)
#endif


class AbstractSerialPrivate;
#if defined(QSERIALDEVICE_EXPORT)
class QSERIALDEVICE_EXPORT AbstractSerial : public QIODevice
#else
class AbstractSerial : public QIODevice
#endif
{
    Q_OBJECT

Q_SIGNALS:
    void signalStatus(const QString &status, QDateTime current);
    void exception();

    void ctsChanged(bool value);
    void dsrChanged(bool value);
    void ringChanged(bool value);

public:

    /*! \~english
        \enum BaudRateDirectionFlag
        Directions baud rate that supports the class AbstractSerial.
        \note Windows support only \a AllBaud.
    */
    enum BaudRateDirectionFlag {
        InputBaud = 0x0001,  /*!< \~english Input baud rate. */
        OutputBaud = 0x0002, /*!< \~english Output baud rate. */
        AllBaud = InputBaud | OutputBaud, /*!< \~english Input and output baud rate equal. */
    };
    Q_DECLARE_FLAGS(BaudRateDirection, BaudRateDirectionFlag)

    /*! \~english
        \enum BaudRate
        Standard types of speed serial device that supports the class AbstractSerial.
    */
    enum BaudRate {
        BaudRateUndefined = -1, /*!< \~english Undefined speed. */

        BaudRate50,       /*!< \~english Speed 50 bauds. */
        BaudRate75,       /*!< \~english Speed 75 bauds. */
        BaudRate110,      /*!< \~english Speed 110 bauds. */
        BaudRate134,      /*!< \~english Speed 134 bauds. */
        BaudRate150,      /*!< \~english Speed 150 bauds. */
        BaudRate200,      /*!< \~english Speed 200 bauds. */
        BaudRate300,      /*!< \~english Speed 300 bauds. */
        BaudRate600,      /*!< \~english Speed 600 bauds. */
        BaudRate1200,     /*!< \~english Speed 1200 bauds. */
        BaudRate1800,     /*!< \~english Speed 1800 bauds. */
        BaudRate2400,     /*!< \~english Speed 2400 bauds. */
        BaudRate4800,     /*!< \~english Speed 4800 bauds. */
        BaudRate9600,     /*!< \~english Speed 9600 bauds. */
        BaudRate14400,    /*!< \~english Speed 14400 bauds. */
        BaudRate19200,    /*!< \~english Speed 19200 bauds. */
        BaudRate38400,    /*!< \~english Speed 38400 bauds. */
        BaudRate56000,    /*!< \~english Speed 56000 bauds. */
        BaudRate57600,    /*!< \~english Speed 57600 bauds. */
        BaudRate76800,    /*!< \~english Speed 76800 bauds. */
        BaudRate115200,   /*!< \~english Speed 115200 bauds. */
        BaudRate128000,   /*!< \~english Speed 128000 bauds. */
        BaudRate230400,   /*!< \~english Speed 230400 bauds. */     //enhanced speed (experimental)
        BaudRate256000,   /*!< \~english Speed 256000 bauds. */
        BaudRate460800,   /*!< \~english Speed 460800 bauds. */     //enhanced speed (experimental)
        BaudRate500000,   /*!< \~english Speed 500000 bauds. */     //enhanced speed (experimental)
        BaudRate576000,   /*!< \~english Speed 576000 bauds. */     //enhanced speed (experimental)
        BaudRate921600,   /*!< \~english Speed 921600 bauds. */     //enhanced speed (experimental)
        BaudRate1000000,  /*!< \~english Speed 1000000 bauds. */    //enhanced speed (experimental)
        BaudRate1152000,  /*!< \~english Speed 1152000 bauds. */    //enhanced speed (experimental)
        BaudRate1500000,  /*!< \~english Speed 1500000 bauds. */    //enhanced speed (experimental)
        BaudRate2000000,  /*!< \~english Speed 2000000 bauds. */    //enhanced speed (experimental)
        BaudRate2500000,  /*!< \~english Speed 2500000 bauds. */    //enhanced speed (experimental)
        BaudRate3000000,  /*!< \~english Speed 3000000 bauds. */    //enhanced speed (experimental)
        BaudRate3500000,  /*!< \~english Speed 3500000 bauds. */    //enhanced speed (experimental)
        BaudRate4000000,  /*!< \~english Speed 4000000 bauds. */    //enhanced speed (experimental)
    };
    /*! \~english
        \enum DataBits
        Standard types of data bits serial device that supports the class AbstractSerial.
    */
    enum DataBits {
        DataBitsUndefined = -1, /*!< \~english Undefined data bits. */

        DataBits5,  /*!< \~english 5 data bits. */
        DataBits6,  /*!< \~english 6 data bits. */
        DataBits7,  /*!< \~english 7 data bits. */
        DataBits8,  /*!< \~english 8 data bits. */
    };
    /*! \~english
        \enum Parity
        Standard types of parity serial device that supports the class AbstractSerial.
    */
    enum Parity {
        ParityUndefined = -1,   /*!< \~english Parity undefined. */

        ParityNone,  /*!< \~english "None" parity. */
        ParityOdd,   /*!< \~english "Odd" parity. */
        ParityEven,  /*!< \~english "Even" parity. */
        ParityMark,  /*!< \~english "Mark" parity. */
        ParitySpace, /*!< \~english "Space" parity. */
    };
    /*! \~english
        \enum StopBits
        Standard types of stop bits serial device that supports the class AbstractSerial.
    */
    enum StopBits {
        StopBitsUndefined = -1, /*!< \~english Undefined stop bit. */

        StopBits1,      /*!< \~english One stop bit. */
        StopBits1_5,    /*!< \~english Half stop bit. */
        StopBits2,      /*!< \~english Two stop bit. */
    };
    /*! \~english
        \enum Flow
        Standard types of flow control serial device that supports the class AbstractSerial.
    */
    enum Flow {
        FlowControlUndefined = -1,  /*!< \~english Flow control undefined. */

        FlowControlOff,         /*!< \~english Flow control "Off". */
        FlowControlHardware,    /*!< \~english Flow control "Hardware". */
        FlowControlXonXoff,     /*!< \~english Flow control "Xon/Xoff". */
    };
    /*! \~english
        \enum Status
        The statuses of the serial device that supports the class AbstractSerial.
    */
    enum Status {
        /* group of "SUCESS STATES" */
        //all
        ENone                       = 0,    /*!< \~english No errors. */
        ENoneOpen                   = 1,    /*!< \~english Successfully opened. */
        ENoneClose                  = 2,    /*!< \~english Successfully closed. */
        ENoneSetBaudRate            = 3,    /*!< \~english Type speed successfully changed. */
        ENoneSetParity              = 4,    /*!< \~english Type of parity was successfully changed. */
        ENoneSetDataBits            = 5,    /*!< \~english Type of data bits successfully changed. */
        ENoneSetStopBits            = 6,    /*!< \~english Type of stop bits successfully changed. */
        ENoneSetFlow                = 7,    /*!< \~english Type of flow control was successfully changed. */
        ENoneSetCharTimeout         = 8,    /*!< \~english Timeout waiting symbol was successfully changed. */
        ENoneSetDtr                 = 9,    /*!< \~english DTR successfully changed. */
        ENoneSetRts                 = 10,   /*!< \~english RTS successfully changed.  */
        ENoneLineStatus             = 11,   /*!< \~english Status lines successfully get. */
        ENoneSendBreak              = 12,   /*!< \~english Send break successfully. */
        ENoneSetBreak               = 13,   /*!< \~english Set break successfully. */
        ENoneFlush                  = 14,   /*!< \~english Flush successfully. */
        ENoneReset                  = 15,   /*!< \~english Reset successfully. */
        // 16-31 reserved

        /* Groups of "ERROR STATES" */

        //group of "OPEN"
        EOpen                       = 32,   /*!< \~english Error opening. */
        EDeviceIsNotOpen            = 33,   /*!< \~english Not open. */
        EOpenModeUnsupported        = 34,   /*!< \~english Open mode is not supported. */
        EOpenModeUndefined          = 35,   /*!< \~english Opening mode undefined. */
        EOpenInvalidFD              = 36,   /*!< \~english Not Actual descriptor. */
        EOpenOldSettingsNotSaved    = 37,   /*!< \~english Failed saving the old parameters when opening. */
        EOpenGetCurrentSettings     = 38,   /*!< \~english Failed to get the current settings when you open. */
        EOpenSetDefaultSettings     = 39,   /*!< \~english Error changing the default settings when you open. */
        EDeviceIsOpen               = 40,   /*!< \~english Already open. */

        //group of "CLOSE"
        ECloseSetOldSettings        = 41,   /*!< \~english Failed saving the old settings when closing. */
        ECloseFD                    = 42,   /*!< \~english Error closing descriptor. */
        EClose                      = 43,   /*!< \~english Error closing. */
        // 44-63 reserved

        //group of "SETTINGS"
        ESetBaudRate                = 64,   /*!< \~english Error change the type of speed. */
        ESetDataBits                = 65,   /*!< \~english Error change the type of data bits. */
        ESetParity                  = 66,   /*!< \~english Error changing the type of parity. */
        ESetStopBits                = 67,   /*!< \~english Error changing the type of stop bits. */
        ESetFlowControl             = 68,   /*!< \~english Error changing the type of flow control. */
        ESetCharIntervalTimeout     = 69,   /*!< \~english Error changing the maximum time allowed to elapse between the arrival of two bytes on the communications line. */
        ESetReadTotalTimeout        = 70,   /*!< \~english Error changing the constant used to calculate the total time-out period for read operations. */
        // 71-95 reserved

        //group of "CONTROL"
        EBytesAvailable             = 96,   /*!< \~english Failed to get number of bytes from the buffer ready for reading. */
        ESetDtr                     = 97,   /*!< \~english Error changing DTR. */
        ESetRts                     = 98,   /*!< \~english Error changing RTS. */
        ELineStatus                 = 99,   /*!< \~english Failed to get status lines. */
        EWaitReadyReadIO            = 100,  /*!< \~english Error I/O waiting to receive data. */
        EWaitReadyReadTimeout       = 101,  /*!< \~english Timeout waiting to receive data. */
        EWaitReadyWriteIO           = 102,  /*!< \~english Error I/O waiting to transfer data.  */
        EWaitReadyWriteTimeout      = 103,  /*!< \~english Timeout waiting to transfer data. */
        EReadDataIO                 = 104,  /*!< \~english Error reading data. */
        EWriteDataIO                = 105,  /*!< \~english Error writing data. */
        EFlush                      = 106,  /*!< \~english Error clearing transmission queue buffer. */
        ESendBreak                  = 107,  /*!< \~english Transmission error of a continuous flow of zero bits. */
        ESetBreak                   = 108,  /*!< \~english Error changing signal discontinuity line. */
        EReset                      = 109,  /*!< \~english Error reset transmission queue buffer. */
        // 110-128 reserved
    };
    /*! \~english
        \enum LineStatusFlag
        Flags of states of the lines: CTS, DSR, DCD, RI, RTS, DTR, ST, SR
        interface serial device (see RS-232 standard, etc.).\n
        To determine the state of the desired line is necessary to impose a mask "and" of the flag line at
        the result of the method: quint16 AbstractSerial::lineStatus().
    */
    enum LineStatusFlag {
        LineLE    = 0x0001, /*!< \~english Line DSR (data set ready/line enable). */
        LineDTR   = 0x0002, /*!< \~english Line DTR (data terminal ready). */
        LineRTS   = 0x0004, /*!< \~english Line RTS (request to send). */
        LineST    = 0x0008, /*!< \~english Line Secondary TXD (transmit). */
        LineSR    = 0x0010, /*!< \~english Line Secondary RXD (receive.) */
        LineCTS   = 0x0020, /*!< \~english Line CTS (clear to send). */
        LineDCD   = 0x0040, /*!< \~english Line DCD (data carrier detect). */
        LineRI    = 0x0080, /*!< \~english Line RNG (ring). */
        LineDSR   = 0x0100, /*!< \~english Line DSR (data set ready). */

        LineErr   = 0x8000, /*!< \~english Error get line status. */
    };
    Q_DECLARE_FLAGS(LineStatus, LineStatusFlag)

    explicit AbstractSerial(QObject *parent = 0);
    virtual ~AbstractSerial();

    void setDeviceName(const QString &deviceName);
    QString deviceName() const;

    bool open(OpenMode mode);
    void close();

    //baud rate
    bool setBaudRate(qint32 baudRate, BaudRateDirection baudDir = AbstractSerial::AllBaud);
    bool setBaudRate(BaudRate baudRate, BaudRateDirection baudDir = AbstractSerial::AllBaud);//overload
    bool setBaudRate(const QString &baudRate, BaudRateDirection baudDir = AbstractSerial::AllBaud);//overload
    QString baudRate(BaudRateDirection baudDir = AbstractSerial::AllBaud) const;
    QStringList listBaudRate() const;
    QMap<AbstractSerial::BaudRate, QString> baudRateMap() const;
    //data bits
    bool setDataBits(DataBits dataBits);
    bool setDataBits(const QString &dataBits);
    QString dataBits() const;
    QStringList listDataBits() const;
    QMap<AbstractSerial::DataBits, QString> dataBitsMap() const;
    //parity
    bool setParity(Parity parity);
    bool setParity(const QString &parity);
    QString parity() const;
    QStringList listParity() const;
    QMap<AbstractSerial::Parity, QString> parityMap() const;
    //stop bits
    bool setStopBits(StopBits stopBits);
    bool setStopBits(const QString &stopBits);
    QString stopBits() const;
    QStringList listStopBits() const;
    QMap<AbstractSerial::StopBits, QString> stopBitsMap() const;
    //flow
    bool setFlowControl(Flow flow);
    bool setFlowControl(const QString &flow);
    QString flowControl() const;
    QStringList listFlowControl() const;
    QMap<AbstractSerial::Flow, QString> flowControlMap() const;
    // Timeouts
    void setCharIntervalTimeout(int usecs = 0);
    int charIntervalTimeout() const;
    void setTotalReadConstantTimeout(int msecs = 0);
    int totalReadConstantTimeout() const;
    //Lines statuses
    bool setDtr(bool set);
    bool setRts(bool set);
    quint16 lineStatus();
    //Break
    bool sendBreak(int duration);
    bool setBreak(bool set);
    //
    bool flush();
    bool reset();

    bool isSequential() const;

    qint64 bytesAvailable() const;
    qint64 bytesToWrite() const;

    bool canReadLine() const;

    bool waitForReadyRead(int msecs = 5000);
    bool waitForBytesWritten(int msecs = 5000);

    //Turns the emit signal change of status (state) devices see enum Status.
    void enableEmitStatus(bool enable);

    qint64 readBufferSize() const;
    void setReadBufferSize(qint64 size);

protected:
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);
    qint64 readLineData(char *data, qint64 maxlen);

    AbstractSerialPrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(AbstractSerial)
    Q_DISABLE_COPY(AbstractSerial)

    void emitStatusString(AbstractSerial::Status status);
    bool canEmitStatusString() const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractSerial::BaudRateDirection)

#ifndef QT_NO_DEBUG_STREAM
#if defined(QSERIALDEVICE_EXPORT)
  //QSERIALDEVICE_EXPORT QDebug operator<<(QDebug debug, quint32 value);
  QSERIALDEVICE_EXPORT QDebug operator<<(QDebug debug, AbstractSerial::BaudRate value);
  QSERIALDEVICE_EXPORT QDebug operator<<(QDebug debug, AbstractSerial::DataBits value);
  QSERIALDEVICE_EXPORT QDebug operator<<(QDebug debug, AbstractSerial::Parity value);
  QSERIALDEVICE_EXPORT QDebug operator<<(QDebug debug, AbstractSerial::StopBits value);
  QSERIALDEVICE_EXPORT QDebug operator<<(QDebug debug, AbstractSerial::Flow value);
  QSERIALDEVICE_EXPORT QDebug operator<<(QDebug debug, AbstractSerial::LineStatus value);
#else
  //QDebug operator<<(QDebug debug, quint32 value);
  QDebug operator<<(QDebug debug, AbstractSerial::BaudRate value);
  QDebug operator<<(QDebug debug, AbstractSerial::DataBits value);
  QDebug operator<<(QDebug debug, AbstractSerial::Parity value);
  QDebug operator<<(QDebug debug, AbstractSerial::StopBits value);
  QDebug operator<<(QDebug debug, AbstractSerial::Flow value);
  QDebug operator<<(QDebug debug, AbstractSerial::LineStatus value);
#endif //QSERIALDEVICE_EXPORT
#endif

#endif // ABSTRACTSERIAL_H
