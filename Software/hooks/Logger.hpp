#ifndef LOGGER_HPP
#define LOGGER_HPP
#include "windows.h"
#include "stdarg.h"
#include "res/logmessages.h"

#define REPORT_LOG_BUF_SIZE 2048
class Logger
{
    static Logger *_this;
public:
    static Logger *getInstance();
    ~Logger();

    void initLog(LPCWSTR name, DWORD logLevel);
    void closeLog();
    void setLogLevel(DWORD logLevel);
    void reportLogDebug(LPCWSTR message, ...);
    void reportLogInfo(LPCWSTR message, ...);
    void reportLogWarning(LPCWSTR message, ...);
    void reportLogError(LPCWSTR message, ...);

protected:
    Logger();

private:
    void reportLog(DWORD type, DWORD msgId, LPCWSTR message, va_list ap);

    HANDLE m_hEventSrc;
    unsigned int m_logLevel;
    LPWSTR m_reportLogBuf;
};

#endif // LOGGER_HPP
