/*
 * D3D10Grabber.cpp
 *
 *  Created on: 29.05.2012
 *     Project: Lightpack
 *
 *  Copyright (c) 2012 Timur Sattarov
 *
 *  Lightpack a USB content-driving ambient lighting system
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "D3D10Grabber.hpp"

#ifdef D3D10_GRAB_SUPPORT

#include <winsock2.h>
#define WINAPI_INLINE WINAPI

#include <QObject>
#include <QThread>
#include <QApplication>
#include <QDesktopWidget>
#include <cstdlib>
#include <stdio.h>
#include "calculations.hpp"
#include "WinUtils.hpp"
#include "WinDXUtils.hpp"
#include "../../common/D3D10GrabberDefs.hpp"
#include "../src/debug.h"
#include "../libraryinjector/ILibraryInjector.h"

#include "../../common/msvcstub.h"
#include <D3D10_1.h>
#include <D3D10.h>

#define SIZEOF_ARRAY(a) (sizeof(a)/sizeof(a[0]))

using namespace std;
using namespace WinUtils;

namespace {
class D3D10GrabberWorker: public QObject {
    Q_OBJECT

public:
    D3D10GrabberWorker(QObject *parent, LPSECURITY_ATTRIBUTES lpsa);
    ~D3D10GrabberWorker();
private:
    HANDLE m_frameGrabbedEvent;
signals:
    void frameGrabbed();
public slots:
    void runLoop();


};

const unsigned kBytesPerPixel = 4;

D3D10GrabberWorker::D3D10GrabberWorker(QObject *parent, LPSECURITY_ATTRIBUTES lpsa) : QObject(parent) {
    if (NULL == (m_frameGrabbedEvent = CreateEventW(lpsa, false, false, HOOKSGRABBER_FRAMEGRABBED_EVENT_NAME))) {
        qCritical() << Q_FUNC_INFO << "unable to create frameGrabbedEvent";
    }
}

D3D10GrabberWorker::~D3D10GrabberWorker() {
    if (m_frameGrabbedEvent)
        CloseHandle(m_frameGrabbedEvent);
}

void D3D10GrabberWorker::runLoop() {
    while(1) {
        if (WAIT_OBJECT_0 == WaitForSingleObject(m_frameGrabbedEvent, 50)) {
            emit frameGrabbed();
            if (!ResetEvent(m_frameGrabbedEvent)) {
                qCritical() << Q_FUNC_INFO << "couldn't reset frameGrabbedEvent";
            }
        }
    }
}
} // namespace

class D3D10GrabberImpl: public QObject
{
    Q_OBJECT

public:
    D3D10GrabberImpl(D3D10Grabber &owner, GrabberContext *context, GetHwndCallback_t getHwndCb)
        : m_sharedMem(NULL),
          m_mutex(NULL),
          m_isStarted(false),
          m_memMap(NULL),
          m_lastFrameId(0),
          m_isInited(false),
          m_libraryInjector(NULL),
          m_isFrameGrabbedDuringLastSecond(false),
          m_context(context),
          m_getHwndCb(getHwndCb),
          m_owner(owner)
    {}

    ~D3D10GrabberImpl()
    {
        shutdown();
    }

    bool initIPC(LPSECURITY_ATTRIBUTES lpsa) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (result != 0) {
            qCritical() << Q_FUNC_INFO << "can't initialize winsocks2. error code " << result;
            return false;
        }

        if (!initSharedMemory(lpsa))
            return false;

        if(!mapFile()) {
            qCritical() << Q_FUNC_INFO << "couldn't create map view. error code " << GetLastError();
            freeIPC();
            return false;
        }

        HOOKSGRABBER_SHARED_MEM_DESC memDesc;
        memset(&memDesc, 0, sizeof(HOOKSGRABBER_SHARED_MEM_DESC));
        if (fillMemoryDesc(&memDesc) && copyMemDesc(memDesc)) {
            memcpy(&m_memDesc, &memDesc, sizeof(HOOKSGRABBER_SHARED_MEM_DESC));
        } else {
            qWarning() << Q_FUNC_INFO << "couldn't write SHARED_MEM_DESC to shared memory!";
        }

        if(!createMutex(lpsa)) {
            qCritical() << Q_FUNC_INFO << "couldn't create mutex.";
            freeIPC();
            return false;
        }

        return true;
    }

    bool init() {
        if(m_isInited)
            return true;

        AcquirePrivileges();
        GetCurrentDirectoryW(SIZEOF_ARRAY(m_hooksLibPath), m_hooksLibPath);
        wcscat(m_hooksLibPath, L"\\");
        wcscat(m_hooksLibPath, lightpackHooksDllName);

        GetWindowsDirectoryW(m_systemrootPath, SIZEOF_ARRAY(m_systemrootPath));

        HRESULT hr = CoInitialize(0);
        //        hr = InitComSecurity();
        //        hr = CoCreateInstanceAsAdmin(NULL, CLSID_ILibraryInjector, IID_ILibraryInjector, reinterpret_cast<void **>(&m_libraryInjector));
        hr = CoCreateInstance(CLSID_ILibraryInjector, NULL, CLSCTX_INPROC_SERVER, IID_ILibraryInjector, reinterpret_cast<void **>(&m_libraryInjector));
        if (FAILED(hr)) {
            qCritical() << Q_FUNC_INFO << "Can't create libraryinjector. D3D10Grabber wasn't initialised. Please try to register server: regsvr32 libraryinjector.dll";
            return false;
        }

#if 0
        // TODO: Remove this code or use |sa| in initIPC()
        SECURITY_ATTRIBUTES sa;
        SECURITY_DESCRIPTOR sd;

        PVOID  ptr;

        // build a restricted security descriptor
        ptr = BuildRestrictedSD(&sd);
        if (!ptr) {
            qCritical() << Q_FUNC_INFO << "Can't create security descriptor. D3D10Grabber wasn't initialised.";
            return;
        }

        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = &sd;
        sa.bInheritHandle = FALSE;
#endif

        if (!initIPC(NULL)) {
            freeIPC();
            qCritical() << Q_FUNC_INFO << "Can't init interprocess communication mechanism. D3D10Grabber wasn't initialised.";
            return false;
        }

#if 0
        FreeRestrictedSD(ptr);
#endif
        m_processesScanAndInfectTimer.reset(new QTimer(this));
        m_processesScanAndInfectTimer->setInterval(5000);
        m_processesScanAndInfectTimer->setSingleShot(false);
        connect(m_processesScanAndInfectTimer.data(), SIGNAL(timeout()), this, SLOT(infectCleanDxProcesses()) );
        m_processesScanAndInfectTimer->start();

        m_workerThread.reset(new QThread());
        m_worker.reset(new D3D10GrabberWorker(NULL, NULL));
        m_worker->moveToThread(m_workerThread.data());
        m_workerThread->start();
        connect(m_worker.data(), SIGNAL(frameGrabbed()), this, SIGNAL(frameGrabbed()), Qt::QueuedConnection);
        QMetaObject::invokeMethod(m_worker.data(), "runLoop", Qt::QueuedConnection);

        m_checkIfFrameGrabbedTimer.reset(new QTimer());
        m_checkIfFrameGrabbedTimer->setSingleShot(false);
        m_checkIfFrameGrabbedTimer->setInterval(1000);
        connect(m_checkIfFrameGrabbedTimer.data(), SIGNAL(timeout()), SLOT(handleIfFrameGrabbed()));
        m_checkIfFrameGrabbedTimer->start();
        m_isInited = true;
        return m_isInited;
    }
    void start() { m_isStarted = true; }
    void stop() { m_isStarted = false; }
    bool isStarted() const { return m_isStarted; }

    QList< ScreenInfo > * screensWithWidgets(QList< ScreenInfo > * result, const QList<GrabWidget *> &grabWidgets)
    {
        Q_UNUSED(grabWidgets);

        result->clear();
        return result;
    }

    GrabResult grabScreens(QList<GrabbedScreen> &grabbedScreens)
    {
        if (!m_isInited) {
            return GrabResultFrameNotReady;
        }

        GrabResult result;
        memcpy(&m_memDesc, m_memMap, sizeof(m_memDesc));
        if( m_memDesc.frameId != HOOKSGRABBER_BLANK_FRAME_ID) {
            if( m_memDesc.frameId != m_lastFrameId) {
                m_lastFrameId = m_memDesc.frameId;

                if ((int)m_memDesc.width != grabbedScreens[0].screenInfo.rect.width() ||
                    (int)m_memDesc.height != grabbedScreens[0].screenInfo.rect.height())
                {
                    qCritical() << Q_FUNC_INFO << "illegal state, screens don't match: qt " <<
                                   grabbedScreens[0].screenInfo.rect << ", remote proc's " <<
                                   m_memDesc.width << "x" << m_memDesc.height;

                    result = GrabResultError;
                } else {
                    if (m_memMap == NULL)
                    {
                        qCritical() << Q_FUNC_INFO << "m_memMap == NULL";
                        return GrabResultError;
                    }

                    grabbedScreens[0].imgData = reinterpret_cast<unsigned char *>(m_memMap) + sizeof(HOOKSGRABBER_SHARED_MEM_DESC);
                    grabbedScreens[0].imgDataSize = m_memDesc.height * m_memDesc.width * kBytesPerPixel;
                    grabbedScreens[0].imgFormat = m_memDesc.format;
                    m_isFrameGrabbedDuringLastSecond = true;
                    result = GrabResultOk;
                }

            } else {
                result = GrabResultFrameNotReady;
            }
        } else {
            result = GrabResultFrameNotReady;
        }

        return result;
    }
signals:
    void frameGrabbed();

private slots:
    void infectCleanDxProcesses(void) {
        if (m_isInited && m_libraryInjector) {
            QList<DWORD> processes = QList<DWORD>();
            getDxProcessesIDs(&processes, m_systemrootPath);
            foreach (DWORD procId, processes) {
                m_libraryInjector->Inject(procId, m_hooksLibPath);
            }
        }
    }

    void handleIfFrameGrabbed() {
        if (!m_isFrameGrabbedDuringLastSecond) {
            if (m_isStarted) {
                emit m_owner.grabberStateChangeRequested(false);
            }
        } else {
            m_isFrameGrabbedDuringLastSecond = false;
        }
    }

private:
    QRgb getColor(const QRect &widgetRect)
    {
        static const QRgb kBlackColor = qRgb(0,0,0);
        DEBUG_HIGH_LEVEL << Q_FUNC_INFO << Debug::toString(widgetRect);

        if (m_memMap == NULL)
        {
            qCritical() << Q_FUNC_INFO << "m_memMap == NULL";
            return 0;
        }

        unsigned char * pbPixelsBuff = reinterpret_cast<unsigned char *>(m_memMap) + sizeof(HOOKSGRABBER_SHARED_MEM_DESC);
        RECT rcMonitor = m_monitorInfo.rcMonitor;
        QRect monitorRect = QRect( QPoint(rcMonitor.left, rcMonitor.top), QSize(m_memDesc.width, m_memDesc.height));

        QRect clippedRect = monitorRect.intersected(widgetRect);

        // Checking for the 'grabme' widget position inside the monitor that is used to capture color
        if( !clippedRect.isValid() ){

            DEBUG_MID_LEVEL << "Widget 'grabme' is out of screen:" << Debug::toString(clippedRect);

            // Widget 'grabme' is out of screen
            return kBlackColor;
        }

        // Convert coordinates from "Main" desktop coord-system to capture-monitor coord-system
        QRect preparedRect = clippedRect.translated(-monitorRect.x(), -monitorRect.y());

        // Align width by 4 for accelerated calculations
        preparedRect.setWidth(preparedRect.width() - (preparedRect.width() % 4));

        if( !preparedRect.isValid() ){
            qWarning() << Q_FUNC_INFO << " preparedRect is not valid:" << Debug::toString(preparedRect);

            // width and height can't be negative
            return kBlackColor;
        }

        QRgb result;

        if (Grab::Calculations::calculateAvgColor(&result, pbPixelsBuff, m_memDesc.format, m_memDesc.rowPitch, preparedRect) == 0) {
            return result;
        } else {
            return kBlackColor;
        }

    #if 0
        if (screenWidth < 1920 && (r > 120 || g > 120 || b > 120)) {
            int monitorWidth = screenWidth;
            int monitorHeight = screenHeight;
            const int BytesPerPixel = 4;
            // Save image of screen:
            QImage * im = new QImage( monitorWidth, monitorHeight, QImage::Format_RGB32 );
            for(int i=0; i<monitorWidth; i++){
                for(int j=0; j<monitorHeight; j++){
                    index = (BytesPerPixel * j * monitorWidth) + (BytesPerPixel * i);
                    QRgb rgb = pbPixelsBuff[index+2] << 16 | pbPixelsBuff[index+1] << 8 | pbPixelsBuff[index];
                    im->setPixel(i, j, rgb);
                }
            }
            im->save("screen.jpg");
            delete im;
        }
    #endif
    }

    void freeIPC()
    {
        if(m_memMap) {
            UnmapViewOfFile(m_memMap);
        }

        if(m_sharedMem) {
            CloseHandle(m_sharedMem);
        }

        if(m_mutex) {
            CloseHandle(m_mutex);
        }
    }

    bool initSharedMemory(LPSECURITY_ATTRIBUTES lpsa)
    {
        m_sharedMem = CreateFileMappingW(INVALID_HANDLE_VALUE, lpsa, PAGE_READWRITE, 0, HOOKSGRABBER_SHARED_MEM_SIZE, HOOKSGRABBER_SHARED_MEM_NAME );
        if(!m_sharedMem) {
            m_sharedMem = OpenFileMappingW(GENERIC_READ | GENERIC_WRITE, false, HOOKSGRABBER_SHARED_MEM_NAME);
            if(!m_sharedMem) {
                qCritical() << Q_FUNC_INFO << "couldn't create memory mapped file. error code " << GetLastError();
            }
        }

        return NULL != m_sharedMem;
    }

    bool mapFile()
    {
        if (!m_sharedMem)
            return false;

        m_memMap = MapViewOfFile(m_sharedMem, FILE_MAP_READ, 0, 0, HOOKSGRABBER_SHARED_MEM_SIZE );
        if (!m_memMap)
            qWarning() << Q_FUNC_INFO << "couldn't map shared memory. error code " << GetLastError();

        return NULL != m_memMap;
    }

    bool fillMemoryDesc(HOOKSGRABBER_SHARED_MEM_DESC *memDesc)
    {
        if (!m_getHwndCb) {
            qWarning() << Q_FUNC_INFO << "m_getHwndCb is NULL!";
            return false;
        }

        memDesc->frameId = HOOKSGRABBER_BLANK_FRAME_ID;

        HWND wnd = (HWND)m_getHwndCb();
        memDesc->d3d9PresentFuncOffset = GetD3D9PresentOffset(wnd);
        memDesc->d3d9SCPresentFuncOffset = GetD3D9SCPresentOffset(wnd);
        memDesc->dxgiPresentFuncOffset = GetDxgiPresentOffset(wnd);

        //converting logLevel from our app's level to EventLog's level
        memDesc->logLevel =  Debug::HighLevel - g_debugLevel;
        return true;
    }

    bool copyMemDesc(const HOOKSGRABBER_SHARED_MEM_DESC &memDesc) const
    {
        if (!m_sharedMem)
            return false;

        PVOID memMap = MapViewOfFile(m_sharedMem, FILE_MAP_WRITE, 0, 0, HOOKSGRABBER_SHARED_MEM_SIZE );
        if(!memMap) {
            qWarning() << Q_FUNC_INFO << "couldn't clear shared memory. error code " << GetLastError();
        } else {
            memcpy(memMap, &memDesc, sizeof(m_memDesc));
            UnmapViewOfFile(memMap);
        }

        return NULL != memMap;
    }

    bool createMutex(LPSECURITY_ATTRIBUTES lpsa)
    {
        m_mutex = CreateMutexW(lpsa, FALSE, HOOKSGRABBER_MUTEX_MEM_NAME);
        if(!m_mutex) {
            m_mutex = OpenMutexW(SYNCHRONIZE, FALSE, HOOKSGRABBER_MUTEX_MEM_NAME);
            if(!m_mutex) {
                qCritical() << Q_FUNC_INFO << "couldn't create mutex. error code " << GetLastError();
            }
        }

        return NULL != m_mutex;
    }

    void shutdown()
    {
        if (!m_isInited)
            return;

        disconnect(this, SLOT(handleIfFrameGrabbed()));
        if (m_libraryInjector)
            m_libraryInjector->Release();
        freeIPC();
        CoUninitialize();
        m_isInited = false;
    }

    HANDLE m_sharedMem;
    HANDLE m_mutex;
    bool m_isStarted;
    PVOID m_memMap;
    UINT m_lastFrameId;
    MONITORINFO m_monitorInfo;
    QScopedPointer<QTimer> m_processesScanAndInfectTimer;
    bool m_isInited;
    ILibraryInjector * m_libraryInjector;
    WCHAR m_hooksLibPath[300];
    WCHAR m_systemrootPath[300];
    bool m_isFrameGrabbedDuringLastSecond;
    GrabberContext *m_context;
    GetHwndCallback_t m_getHwndCb;

    QScopedPointer<QTimer> m_checkIfFrameGrabbedTimer;
    QScopedPointer<D3D10GrabberWorker> m_worker;
    QScopedPointer<QThread> m_workerThread;
    HOOKSGRABBER_SHARED_MEM_DESC m_memDesc;
    D3D10Grabber &m_owner;
};

// This will force qmake and moc to process internal classes in this file.
#include "D3D10Grabber.moc"

D3D10Grabber::D3D10Grabber(QObject *parent, GrabberContext *context, GetHwndCallback_t getHwndCb)
    : GrabberBase(parent, context) {
    m_impl.reset(new D3D10GrabberImpl(*this, context, getHwndCb));
}

void D3D10Grabber::init() {
    m_impl->init();
    connect(m_impl.data(), SIGNAL(frameGrabbed()), this, SLOT(grab()));
    _screensWithWidgets.clear();
    GrabbedScreen grabbedScreen;
    grabbedScreen.screenInfo.handle = reinterpret_cast<void *>(QApplication::desktop()->primaryScreen());
    grabbedScreen.screenInfo.rect = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
    _screensWithWidgets.append(grabbedScreen);
}

void D3D10Grabber::startGrabbing() {
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << this->metaObject()->className();
    m_impl->start();
}

void D3D10Grabber::stopGrabbing() {
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << this->metaObject()->className();
    m_impl->stop();
}

void D3D10Grabber::setGrabInterval(int msec) {
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << this->metaObject()->className();
    Q_UNUSED(msec);
}

/*!
 * Just stub, we don't need to reallocate anything, and we suppose fullscreen application
 * runs on primary screen \see D3D10Grabber#init()
 * \param result
 * \param grabWidgets
 * \return
 */
QList< ScreenInfo > * D3D10Grabber::screensWithWidgets(QList< ScreenInfo > * result, const QList<GrabWidget *> &grabWidgets)
{
    Q_UNUSED(grabWidgets);

    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << this->metaObject()->className();
    return result;
}

/*!
 * Just stub. Reallocations are unnecessary here.
 * \param grabScreens
 * \return
 */
bool D3D10Grabber::reallocate(const QList<ScreenInfo> &grabScreens)
{
    Q_UNUSED(grabScreens);

    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << this->metaObject()->className();
    return true;
}

GrabResult D3D10Grabber::grabScreens()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << this->metaObject()->className();
    if (m_impl && m_impl->isStarted()) {
        GrabResult result = m_impl->grabScreens(_screensWithWidgets);
        return result;
    } else {
        emit grabberStateChangeRequested(true);
        return GrabResultFrameNotReady;
    }
}

D3D10Grabber::~D3D10Grabber() {
}

bool D3D10Grabber::isGrabbingStarted() const {
    return m_impl->isStarted();
}

#endif
