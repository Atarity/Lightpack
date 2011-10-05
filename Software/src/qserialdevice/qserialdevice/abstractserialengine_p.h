#ifndef ABSTRACTSERIALENGINE_P_H
#define ABSTRACTSERIALENGINE_P_H

#include "abstractserialengine.h"


class AbstractSerialEnginePrivate
{
public:
    AbstractSerialEnginePrivate();

    static const char defaultDeviceName[];

    QString deviceName;
    qint32 ibaudRate;
    qint32 obaudRate;
    AbstractSerial::DataBits dataBits;
    AbstractSerial::Parity parity;
    AbstractSerial::StopBits stopBits;
    AbstractSerial::Flow flow;
    int charIntervalTimeout;
    AbstractSerial::Status status;
    bool oldSettingsIsSaved;
    //
    Serial::DESCRIPTOR descriptor;
    AbstractSerialEngineReceiver *receiver;
    bool isAutoCalcReadTimeoutConstant;

    //For auto recalc timeout value
    int currDataBitsToValue() const;
    int currParityToValue() const;
    int currStopBitsToValue() const;

};

#endif // ABSTRACTSERIALENGINE_P_H
