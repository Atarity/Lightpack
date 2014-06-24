#include "ProxyFuncJmp.hpp"
#include "hooksutils.h"
#include "string.h"
#include <stdint.h>

bool ProxyFuncJmp::init() {
    if (!m_isInited) {
        if(m_pTarget && m_pSubstFunc) {
            if (m_logger)
                m_logger->reportLogDebug(L"JmpFuncInitialization. m_pTarget=0x%x, m_pSubstFunc=0x%x", m_pTarget, m_pSubstFunc);
        } else {
            if (m_logger)
                m_logger->reportLogError(L"JmpFuncInitialization. m_pTarget=null or m_pSubstFunc=null");
        }

        m_pJmpCode[0] = 0x90; // for debug replace with 0xcc (int 3h)
        m_pJmpCode[1] = 0xe9;
        int relativeSubstFunc = reinterpret_cast<uintptr_t>(m_pSubstFunc) -
                reinterpret_cast<uintptr_t>(m_pTarget) - kJmpInstructionSize;
        memcpy(m_pJmpCode+2, &relativeSubstFunc, sizeof(relativeSubstFunc));

        memcpy(m_pOriginalCode, m_pTarget, kJmpInstructionSize);

        m_isInited = true;
    }
    return m_isInited;
}

bool ProxyFuncJmp::isHookInstalled() {
    return m_isInited && memcmp(m_pTarget, m_pOriginalCode, kJmpInstructionSize)!=0;
}

bool ProxyFuncJmp::installHook() {
    return m_isInited && WriteToProtectedMem(m_pTarget, m_pJmpCode, NULL, kJmpInstructionSize);
}

bool ProxyFuncJmp::removeHook() {
    return m_isInited && WriteToProtectedMem(m_pTarget, m_pOriginalCode, NULL, kJmpInstructionSize);
}
