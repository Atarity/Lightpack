#ifndef HOOKSUTILS_H
#define HOOKSUTILS_H

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <stdarg.h>

//void InitLog(const LPWSTR name, DWORD logLevel);
//void CloseLog();
//void SetLogLevel(DWORD logLevel);
//void ReportLogDebug(const LPWSTR message, ...);
//void ReportLogInfo(const LPWSTR message, ...);
//void ReportLogWarning(const LPWSTR message, ...);
//void ReportLogError(const LPWSTR message, ...);

bool WriteToProtectedMem(void * mem, void * newVal, void * savedVal, size_t size);

inline void * incPtr(void * ptr, UINT offset) {
    return (void *)( (DWORD_PTR)ptr + offset );
}

#endif // HOOKSUTILS_H
