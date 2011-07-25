#ifndef DEFS_H
#define DEFS_H
#include<qglobal.h>

#define QT_GRAB_SUPPORT
#ifdef Q_WS_WIN
#   define WINAPI_GRAB_SUPPORT
#elif defined(Q_OS_UNIX)
#   define X11_GRAB_SUPPORT
#endif

#if defined(Q_OS_DARWIN) || defined(Q_OS_MAC) || defined(Q_OS_MACX)
#   undef X11_GRAB_SUPPORT
#endif

#endif // DEFS_H
