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

    void initLog(LPWSTR name, DWORD logLevel);
    void closeLog();
    void setLogLevel(DWORD logLevel);
    void reportLogDebug(const LPWSTR message, ...);
    void reportLogInfo(const LPWSTR message, ...);
    void reportLogWarning(const LPWSTR message, ...);
    void reportLogError(const LPWSTR message, ...);

protected:
    Logger();

private:

    void reportLog(DWORD type, DWORD msgId, const LPWSTR message, va_list ap);

    HANDLE m_hEventSrc;
    unsigned int m_logLevel;
    LPWSTR m_reportLogBuf;
};

#endif // LOGGER_HPP
