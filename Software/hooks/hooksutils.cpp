#include "hooksutils.h"
#include "Logger.hpp"
//static HANDLE ghEventSrc = NULL;
//LPWSTR gReportLogBuf = NULL;
//static DWORD gLogLevel = 3;

//void ReportLog(DWORD type, DWORD msgId, const LPWSTR message, va_list ap);

//void InitLog(LPWSTR name, DWORD logLevel) {
//    if (!ghEventSrc) ghEventSrc = RegisterEventSourceW(NULL, name);
//    if (!gReportLogBuf) gReportLogBuf = (WCHAR*)malloc(REPORT_LOG_BUF_SIZE);
//    gLogLevel = logLevel;
//}

//void SetLogLevel(DWORD logLevel) {
//    gLogLevel = logLevel;
//}

//void ReportLogDebug(const LPWSTR message, ...) {
//    va_list ap;
//    va_start( ap, message );
//    if (gLogLevel <= PRISMATIK_LOG_SEVERITY_DEBUG) {
//        ReportLog(EVENTLOG_INFORMATION_TYPE, PRISMATIK_LOG_MSG_DEBUG, message, ap);
//    }
//    va_end( ap );
//}

//void ReportLogInfo(const LPWSTR message, ...) {
//    va_list ap;
//    va_start( ap, message );
//    if (gLogLevel <= PRISMATIK_LOG_SEVERITY_INFO) {
//        ReportLog(EVENTLOG_INFORMATION_TYPE, PRISMATIK_LOG_MSG_INFO, message, ap);
//    }
//    va_end( ap );
//}

//void ReportLogWarning(const LPWSTR message, ...) {
//    va_list ap;
//    va_start( ap, message );
//    if (gLogLevel <= PRISMATIK_LOG_SEVERITY_WARNING) {
//        ReportLog(EVENTLOG_WARNING_TYPE, PRISMATIK_LOG_MSG_WARNING, message, ap);
//    }
//    va_end( ap );
//}

//void ReportLogError(const LPWSTR message, ...) {
//    va_list ap;
//    va_start( ap, message );
//        ReportLog(EVENTLOG_ERROR_TYPE, PRISMATIK_LOG_MSG_ERROR, message, ap);
//    va_end( ap );
//}

//void ReportLog(DWORD type, DWORD msgId, const LPWSTR message, va_list ap) {
//    if (ghEventSrc) {
//        ZeroMemory(gReportLogBuf, sizeof(gReportLogBuf));
//        int sprintfResult = wvsprintfW(gReportLogBuf, message, ap);
//        if (sprintfResult > -1)
//            ReportEventW(ghEventSrc, type, 0, msgId, NULL, 1, 0, const_cast<const WCHAR **>(&gReportLogBuf), NULL);
//    }
//}

//void CloseLog() {
//    if (ghEventSrc) {
//        DeregisterEventSource(ghEventSrc);
//        ghEventSrc = NULL;
//    }
//    if (gReportLogBuf) {
//        free(gReportLogBuf);
//        gReportLogBuf = NULL;
//    }
//}

bool WriteToProtectedMem(void * mem, void * newVal, void * savedVal, size_t size) {
    DWORD protection = PAGE_READWRITE;
    DWORD oldProtection;
    Logger *logger = Logger::getInstance();

    // align protected/unprotected memory by 8 bytes
    size_t protectedSize = ((size + 7)/8)*8;

    if (VirtualProtect(mem, protectedSize, protection, &oldProtection)) {
        if (savedVal) {
            logger->reportLogDebug(L"Writing to memory. Saving old content. dest 0x%x, src 0x%x, size %u", savedVal, mem, size);
            memcpy(savedVal, mem, size);
            logger->reportLogDebug(L"Writing to memory. Saving old content. finished");
        }
        logger->reportLogDebug(L"Writing to memory. dest 0x%x, src 0x%x, size %u", mem, newVal, size);
        memcpy(mem, newVal, size);
        logger->reportLogDebug(L"Writing to memory. finished");

        if (VirtualProtect(mem, protectedSize, oldProtection, &oldProtection))
            return true;
    }
    return false;
}
