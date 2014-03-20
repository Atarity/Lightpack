#include "EndSessionDetector.hpp"
#include "debug.h"
#include "LightpackApplication.hpp"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

EndSessionDetector::EndSessionDetector()
{
}

bool EndSessionDetector::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(result);
    Q_UNUSED(eventType);

    bool isSessionEnding = false;

#ifdef Q_OS_WIN
    MSG* msg = (MSG*)message;
    
    if (msg->message == WM_QUERYENDSESSION)
    {
        isSessionEnding = true;
    }
#endif

    if (isSessionEnding)
        getLightpackApp()->settingsWnd()->switchOffLeds();

    return false;
}

EndSessionDetector::~EndSessionDetector()
{
}
