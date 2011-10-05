
#include <QtCore/QSocketNotifier>
#include <sys/ioctl.h>

#include "nativeserialnotifier.h"


NativeSerialNotifier::NativeSerialNotifier(NativeSerialEngine *parent)
        : QThread(parent)
        , readNotifier(0)
        , writeNotifier(0)
        , exceptionNotifier(0)
        , running(false)
{
    this->engine = parent;
}

NativeSerialNotifier::~NativeSerialNotifier()
{
    if (this->readNotifier)
        this->readNotifier->setEnabled(false);
    if (this->writeNotifier)
        this->writeNotifier->setEnabled(false);
    if (this->exceptionNotifier)
        this->exceptionNotifier->setEnabled(false);

    this->running = false;
    wait(1000);
}

bool NativeSerialNotifier::isReadNotificationEnabled() const
{
    return (this->readNotifier && this->readNotifier->isEnabled());
}

void NativeSerialNotifier::setReadNotificationEnabled(bool enable)
{
    if (this->readNotifier) {
        this->readNotifier->setEnabled(enable);
    }
    else {
        if (enable) {
            this->readNotifier = new QSocketNotifier(this->engine->descriptor(), QSocketNotifier::Read, this);
            this->readNotifier->installEventFilter(this);
            this->readNotifier->setEnabled(true);
        }
    }
}

bool NativeSerialNotifier::isWriteNotificationEnabled() const
{
    return (this->writeNotifier && this->writeNotifier->isEnabled());
}

void NativeSerialNotifier::setWriteNotificationEnabled(bool enable)
{
    if (this->writeNotifier) {
        this->writeNotifier->setEnabled(enable);
    }
    else {
        if (enable) {
            this->writeNotifier = new QSocketNotifier(this->engine->descriptor(), QSocketNotifier::Write, this);
            this->writeNotifier->installEventFilter(this);
            this->writeNotifier->setEnabled(true);
        }
    }
}

bool NativeSerialNotifier::isExceptionNotificationEnabled() const
{
    return (this->exceptionNotifier && this->exceptionNotifier->isEnabled());
}

void NativeSerialNotifier::setExceptionNotificationEnabled(bool enable)
{
    if (this->exceptionNotifier) {
        this->exceptionNotifier->setEnabled(enable);
    }
    else {
        if (enable) {
            this->exceptionNotifier = new QSocketNotifier(this->engine->descriptor(), QSocketNotifier::Exception, this);
            this->exceptionNotifier->installEventFilter(this);
            this->exceptionNotifier->setEnabled(true);
        }
    }
}

bool NativeSerialNotifier::isLineNotificationEnabled() const
{
    return this->isRunning();
}

void NativeSerialNotifier::setLineNotificationEnabled(bool enable)
{
    if (enable && (!this->isRunning())) {
        this->running = true;
        this->start();
    }
    else
        this->running = false;
}

bool NativeSerialNotifier::eventFilter(QObject *obj, QEvent *e)
{
    if ((obj == this->readNotifier) && (e->type() == QEvent::SockAct)) {
        this->engine->readNotification();
        return true;
    }
    if ((obj == this->writeNotifier) && (e->type() == QEvent::SockAct)) {
        this->engine->writeNotification();
        return true;
    }
    if ((obj == this->exceptionNotifier) && (e->type() == QEvent::SockAct)) {
        this->engine->exceptionNotification();
        return true;
    }
    return QThread::eventFilter(obj, e);
}

void NativeSerialNotifier::run()
{
#if defined (TIOCMGET)
    static int oldLine = 0;
    int currLine = 0;
    while (this->running) {
        if ((-1 != ::ioctl(this->engine->descriptor(), TIOCMGET, &currLine))
            && (currLine != oldLine)) {

            oldLine = currLine;
            this->engine->lineNotification();
        }
        this->msleep(3);
    }
#endif
}
