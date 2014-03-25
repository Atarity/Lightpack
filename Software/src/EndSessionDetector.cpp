#include "EndSessionDetector.hpp"
#include "debug.h"
#include "LightpackApplication.hpp"

#include <exception>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <WtsApi32.h>
#endif

EndSessionDetector::EndSessionDetector()
	: m_isDestroyed(false)
{
#ifdef Q_OS_WIN
	if (WTSRegisterSessionNotification(getLightpackApp()->getMainWindowHandle(), NOTIFY_FOR_THIS_SESSION) == FALSE)
		throw std::exception("Failed to register session notification.");
#endif
}

bool EndSessionDetector::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
	Q_UNUSED(result);
	Q_UNUSED(eventType);

	bool isSessionEnding = false;
	bool isSessionResuming = false;

#ifdef Q_OS_WIN
	MSG* msg = (MSG*)message;

	if (msg->message == WM_QUERYENDSESSION)
	{
		if (SettingsScope::Settings::isKeepLightsOnAfterExit())
		{
			Destroy();
			isSessionEnding = true;
		}
	}
	else if (msg->message == WM_WTSSESSION_CHANGE)
	{
		if (SettingsScope::Settings::isKeepLightsOnAfterLock())
		{
			if (msg->wParam == WTS_SESSION_LOCK)
			{
				isSessionEnding = true;
			}
			else if (msg->wParam == WTS_SESSION_UNLOCK)
			{
				isSessionResuming = true;
			}
		}
	}
#endif

	if (isSessionEnding)
		getLightpackApp()->settingsWnd()->switchOffLeds();
	else if (isSessionResuming)
		getLightpackApp()->settingsWnd()->switchOnLeds();

	return false;
}

void EndSessionDetector::Destroy()
{
	if (!m_isDestroyed)
	{
		m_isDestroyed = true;
		WTSUnRegisterSessionNotification(getLightpackApp()->getMainWindowHandle());
	}
}

EndSessionDetector::~EndSessionDetector()
{
	Destroy();
}
