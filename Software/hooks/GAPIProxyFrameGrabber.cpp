#include "GAPIProxyFrameGrabber.hpp"
#include "hooksutils.h"
#include "IPCContext.hpp"

GAPIProxyFrameGrabber::GAPIProxyFrameGrabber(HANDLE syncRunMutex) {
    LARGE_INTEGER liFreq;
    QueryPerformanceFrequency(&liFreq);
    m_liFreq.QuadPart = liFreq.QuadPart / 1000;
    m_pcOverall.QuadPart = 0;
    m_pcOriginal.QuadPart = 0;
    m_syncRunMutex = syncRunMutex;
}

GAPIProxyFrameGrabber::~GAPIProxyFrameGrabber() {
    if (this->isHooksInstalled())
        this->removeHooks();
    this->free();
}

bool GAPIProxyFrameGrabber::startOverallPerfCount() {
    return !!QueryPerformanceCounter(&m_pcStart);
}

bool GAPIProxyFrameGrabber::startOriginalPerfCount() {
    return !!QueryPerformanceCounter(&m_pcIntermediate);
}

bool GAPIProxyFrameGrabber::stopPerfCount() {
    if (QueryPerformanceCounter(&m_pcStop)) {
        m_pcOverall.QuadPart += (m_pcStop.QuadPart - m_pcStart.QuadPart) / m_liFreq.QuadPart;
        m_pcOriginal.QuadPart += (m_pcStop.QuadPart - m_pcIntermediate.QuadPart) / m_liFreq.QuadPart;
        return true;
    } else {
        return false;
    }
}
