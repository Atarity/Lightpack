#ifndef NATIVESERIALNOTIFIER_H
#define NATIVESERIALNOTIFIER_H


#include <QtCore/QEvent>

#ifdef Q_OS_WIN
  #include <QtCore/private/qwineventnotifier_p.h>
#else
  #include <QtCore/QThread>
  class QSocketNotifier;
#endif

#include "abstractserialnotifier.h"


#ifdef Q_OS_WIN
class NativeSerialNotifier : public QWinEventNotifier, public AbstractSerialNotifier
{
public:
    NativeSerialNotifier(NativeSerialEngine *parent);
    virtual ~NativeSerialNotifier();

    bool isReadNotificationEnabled() const;
    void setReadNotificationEnabled(bool enable);
    bool isWriteNotificationEnabled() const;
    void setWriteNotificationEnabled(bool enable);
    bool isExceptionNotificationEnabled() const;
    void setExceptionNotificationEnabled(bool enable);
    bool isLineNotificationEnabled() const;
    void setLineNotificationEnabled(bool enable);

protected:
    bool event(QEvent *e);

private:
    ::OVERLAPPED o;
    ::DWORD currentMask;
    ::DWORD setMask;

    void setMaskAndActivateEvent();
};
#else
class NativeSerialNotifier : public QThread, public AbstractSerialNotifier
{
public:
    NativeSerialNotifier(NativeSerialEngine *parent);
    virtual ~NativeSerialNotifier();

    bool isReadNotificationEnabled() const;
    void setReadNotificationEnabled(bool enable);
    bool isWriteNotificationEnabled() const;
    void setWriteNotificationEnabled(bool enable);
    bool isExceptionNotificationEnabled() const;
    void setExceptionNotificationEnabled(bool enable);
    bool isLineNotificationEnabled() const;
    void setLineNotificationEnabled(bool enable);

protected:
    bool eventFilter(QObject *obj, QEvent *e);
    void run();

private:
    QSocketNotifier *readNotifier;
    QSocketNotifier *writeNotifier;
    QSocketNotifier *exceptionNotifier;
    bool running;
};
#endif

#endif // NATIVESERIALNOTIFIER_H
