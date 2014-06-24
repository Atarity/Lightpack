#include "IPCContext.hpp"

IPCContext::~IPCContext() {
    this->free();
}

bool IPCContext::init() {
    if (NULL == (m_hMutex = OpenMutexW(SYNCHRONIZE, false, HOOKSGRABBER_MUTEX_MEM_NAME))) {
        m_logger->reportLogError(L"error occured while opening mutex 0x%x", GetLastError());
        this->free();
        return false;
    }

    if (NULL == (m_hSharedMem = OpenFileMappingW(GENERIC_WRITE | GENERIC_READ, false, HOOKSGRABBER_SHARED_MEM_NAME))) {
        m_logger->reportLogError(L"error occured while opening memory-mapped file 0x%x", GetLastError());
        this->free();
        return false;
    }

    if (NULL == (m_hFrameGrabbedEvent = CreateEventW(NULL, true, false, HOOKSGRABBER_FRAMEGRABBED_EVENT_NAME))) {
        m_logger->reportLogError(L"error occured while opening event 0x%x", GetLastError());
        this->free();
        return false;
    }

    m_pMemMap = MapViewOfFile(m_hSharedMem, FILE_MAP_WRITE, 0, 0, HOOKSGRABBER_SHARED_MEM_SIZE);
    if (m_pMemMap == NULL) {
        m_logger->reportLogError(L"error occured while creating mapview 0x%x", GetLastError());
        this->free();
        return false;
    }

    PVOID pMemMapReadOnly = MapViewOfFile(m_hSharedMem, FILE_MAP_READ, 0, 0, HOOKSGRABBER_SHARED_MEM_SIZE);
    if (pMemMapReadOnly == NULL) {
        m_logger->reportLogError(L"error occured while creating read-only mapview 0x%x", GetLastError());
        this->free();
        return false;
    }

    memcpy(&m_memDesc, pMemMapReadOnly, sizeof(m_memDesc));

    UnmapViewOfFile(pMemMapReadOnly);

    m_memDesc.frameId = 0;

    m_logger->reportLogInfo(L"d3d9 device::present(): 0x%x", m_memDesc.d3d9PresentFuncOffset);
    m_logger->reportLogInfo(L"d3d9 swapchain::present(): 0x%x", m_memDesc.d3d9SCPresentFuncOffset);
    m_logger->reportLogInfo(L"dxgi swapchain::present(): 0x%x", m_memDesc.dxgiPresentFuncOffset);

    return true;
}

void IPCContext::free() {
    if (m_hMutex)
        CloseHandle(m_hMutex);

    if (m_pMemMap)
        UnmapViewOfFile(m_pMemMap);

    if (m_hSharedMem)
        CloseHandle(m_hSharedMem);

    if (m_hFrameGrabbedEvent)
        CloseHandle(m_hFrameGrabbedEvent);
}
