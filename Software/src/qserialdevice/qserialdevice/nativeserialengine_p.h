#ifndef NATISESERIALENGINE_P_H
#define NATISESERIALENGINE_P_H

#include "nativeserialengine.h"
#include "abstractserialengine_p.h"

#if defined (Q_OS_WIN)
  #include <qt_windows.h>
#else
  #include <termios.h>
  #include "../unix/ttylocker.h"
#endif

#ifndef READ_CHUNKSIZE
#define READ_CHUNKSIZE Q_INT64_C(256)
#endif

#ifndef WRITE_CHUNKSIZE
#define WRITE_CHUNKSIZE Q_INT64_C(512)
#endif

class AbstractSerialNotifier;
class NativeSerialEnginePrivate : public AbstractSerialEnginePrivate
{
public:
    NativeSerialEnginePrivate();

    bool nativeOpen(AbstractSerial::OpenMode mode);
    bool nativeClose();

    bool nativeSetBaudRate(qint32 baudRate, AbstractSerial::BaudRateDirection baudDir);
    bool nativeSetDataBits(AbstractSerial::DataBits dataBits);
    bool nativeSetParity(AbstractSerial::Parity parity);
    bool nativeSetStopBits(AbstractSerial::StopBits stopBits);
    bool nativeSetFlowControl(AbstractSerial::Flow flow);

    void nativeSetCharReadTimeout(int usecs);
    int nativeCharReadTimeout() const;

    void nativeSetTotalReadConstantTimeout(int msecs);
    int nativeTotalReadConstantTimeout() const;

    bool nativeSetDtr(bool set) const;
    bool nativeSetRts(bool set) const;
    quint16 nativeLineStatus() const;

    bool nativeSendBreak(int duration) const;
    bool nativeSetBreak(bool set) const;

    bool nativeFlush() const;
    bool nativeReset() const;

    qint64 nativeBytesAvailable() const;

    qint64 nativeRead(char *data, qint64 len);
    qint64 nativeWrite(const char *data, qint64 len);

    int nativeSelect(int timeout, bool checkRead, bool checkWrite,
                     bool *selectForRead, bool *selectForWrite);

    //add 05.11.2009  (while is not used, reserved)
    qint64 nativeCurrentQueue(NativeSerialEngine::ioQueue queue) const;

    AbstractSerialNotifier *notifier;

private:

#ifdef Q_OS_WIN

    enum CommTimeouts {
        ReadIntervalTimeout, ReadTotalTimeoutMultiplier,
        ReadTotalTimeoutConstant, WriteTotalTimeoutMultiplier,
        WriteTotalTimeoutConstant
    };
    ::DCB dcb, olddcb;
    ::COMMTIMEOUTS ct, oldct;
    ::OVERLAPPED oRead;
    ::OVERLAPPED oWrite;
    ::OVERLAPPED oSelect;

    bool createEvents(bool rx, bool tx);
    bool closeEvents() const;
    void recalcTotalReadTimeoutConstant();
    void prepareCommTimeouts(CommTimeouts cto, ::DWORD msecs);
    bool updateDcb();
    bool updateCommTimeouts();
#else
    struct termios tio;
    struct termios oldtio;
    TTYLocker locker;
    void prepareTimeouts(int msecs);
    bool updateTermious();
    bool setStandartBaud(AbstractSerial::BaudRateDirection, speed_t speed);
    bool setCustomBaud(qint32 speed);
#endif

    void initVariables();

    bool detectDefaultBaudRate();
    bool detectDefaultDataBits();
    bool detectDefaultParity();
    bool detectDefaultStopBits();
    bool detectDefaultFlowControl();
    bool detectDefaultCurrentSettings();
    void prepareOtherOptions();
    bool saveOldSettings();
    bool restoreOldSettings();
};

#endif // NATISESERIALENGINE_P_H
