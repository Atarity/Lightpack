#include "hooksutils.h"
#include "Logger.hpp"

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
