/*
 * D3D10Grabber.hpp
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

#pragma once

#include<GrabberBase.hpp>
#ifdef D3D10_GRAB_SUPPORT
#define WINVER 0x0500

#include<windows.h>
#include"../../common/D3D10GrabberDefs.hpp"
#include<QList>
#include<../libraryinjector/ILibraryInjector.h>

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

class D3D10Grabber : public GrabberBase
{

    Q_OBJECT
public:
    D3D10Grabber(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabWidgets);
    ~D3D10Grabber();

    void init(void);

protected:
    virtual GrabResult _grab();

public slots:
    virtual void updateGrabMonitor(QWidget * firstWidget);
    virtual void startGrabbing();
    virtual void stopGrabbing();
    virtual bool isGrabbingStarted() const { return m_isStarted; }
    virtual void setGrabInterval(int msec);
    virtual void grab();

//    void setFallbackGrabber(GrabberBase * grabber);

private slots:
    void infectCleanDxProcesses(void);
    void handleIfFrameGrabbed();

private:
    bool initIPC(LPSECURITY_ATTRIBUTES lpsa);
    void freeIPC();
    QRgb getColor(QRect &widgetRect);
    QTimer *m_checkIfFrameGrabbedTimer;

    static HANDLE m_sharedMem;
    static HANDLE m_mutex;
    static bool m_isStarted;
    static PVOID m_memMap;
    D3D10GrabberWorker *m_worker;
    QThread * m_workerThread;
//    GrabberBase * m_fallbackGrabber;
    D3D10GRABBER_SHARED_MEM_DESC m_memDesc;
    static UINT m_lastFrameId;
    static MONITORINFO m_monitorInfo;
    static QTimer * m_processesScanAndInfectTimer;
    static bool m_isInited;
    static ILibraryInjector * m_libraryInjector;
    static WCHAR m_hooksLibPath[300];
    static bool m_isFrameGrabbedDuringLastSecond;
};

#endif
