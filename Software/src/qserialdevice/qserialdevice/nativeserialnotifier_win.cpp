#include "nativeserialnotifier.h"


NativeSerialNotifier::NativeSerialNotifier(NativeSerialEngine *parent)
        : QWinEventNotifier(parent)
        , currentMask(0)
        , setMask(0)
{
    this->engine = parent;
    ::memset(&this->o, 0, sizeof(::OVERLAPPED));
    this->o.hEvent = ::CreateEvent(0, false, false, 0);
    this->setHandle(this->o.hEvent);
}

NativeSerialNotifier::~NativeSerialNotifier()
{
    this->setEnabled(false);
    ::CloseHandle(this->o.hEvent);
}

bool NativeSerialNotifier::isReadNotificationEnabled() const
{
    return (this->isEnabled() && (EV_RXCHAR & this->setMask));
}

void NativeSerialNotifier::setReadNotificationEnabled(bool enable)
{
    (enable) ? (this->setMask |= EV_RXCHAR) : (this->setMask &= ~EV_RXCHAR);
    this->setMaskAndActivateEvent();
}

bool NativeSerialNotifier::isWriteNotificationEnabled() const
{
    return (this->isEnabled() && (EV_TXEMPTY & this->setMask));
}

void NativeSerialNotifier::setWriteNotificationEnabled(bool enable)
{
    (enable) ? (this->setMask |= EV_TXEMPTY) : (this->setMask &= ~EV_TXEMPTY);
    this->setMaskAndActivateEvent();

    // This only for OS Windows, as EV_TXEMPTY event is triggered only
    // after the last byte of data (as opposed to events such as Write QSocketNotifier).
    // Therefore, we are forced to run writeNotification(), as EV_TXEMPTY does not work.
    if (enable)
        this->engine->writeNotification();
}

bool NativeSerialNotifier::isExceptionNotificationEnabled() const
{
    return (this->isEnabled() && (EV_ERR & this->setMask));
}

void NativeSerialNotifier::setExceptionNotificationEnabled(bool enable)
{
    (enable) ? (this->setMask |= EV_ERR) : (this->setMask &= ~EV_ERR);
    this->setMaskAndActivateEvent();
}

bool NativeSerialNotifier::isLineNotificationEnabled() const
{
    return (this->isEnabled() && ((EV_CTS | EV_DSR | EV_RING) & this->setMask));
}

void NativeSerialNotifier::setLineNotificationEnabled(bool enable)
{
    ::DWORD lineMask = (EV_CTS | EV_DSR | EV_RING);
    (enable) ? (this->setMask |= lineMask) : (this->setMask &= ~lineMask);
    this->setMaskAndActivateEvent();
}

bool NativeSerialNotifier::event(QEvent *e)
{
    bool ret = false;
    if (e->type() == QEvent::WinEventAct) {

        if (EV_RXCHAR & this->currentMask & this->setMask) {
            this->engine->readNotification();
            ret = true;
        }
        //TODO: This is why it does not work?
        if (EV_TXEMPTY & this->currentMask & this->setMask) {
            this->engine->writeNotification();
            ret = true;
        }
        if (EV_ERR & this->currentMask & this->setMask) {
            this->engine->exceptionNotification();
            ret = true;
        }

        this->currentMask &= (EV_CTS | EV_DSR | EV_RING);
        if (this->currentMask & this->setMask) {
            this->engine->lineNotification();
            ret = true;
        }
    }
    else
        ret = QWinEventNotifier::event(e);

    ::WaitCommEvent(this->engine->descriptor(), &this->currentMask, &this->o);
    return ret;
}

void NativeSerialNotifier::setMaskAndActivateEvent()
{
    ::SetCommMask(this->engine->descriptor(), this->setMask);

    if (this->setMask)
        ::WaitCommEvent(this->engine->descriptor(), &this->currentMask, &this->o);

    switch (this->setMask) {
    case 0:
        if (this->isEnabled())
            this->setEnabled(false);
        break;
    default:
        if (!this->isEnabled())
            this->setEnabled(true);
    }
}
