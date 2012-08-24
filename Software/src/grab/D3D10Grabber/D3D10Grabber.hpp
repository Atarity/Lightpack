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
#include"D3D10GrabberDefs.hpp"
#include<QList>
#include<libraryinjector/ILibraryInjector.h>
class D3D10Grabber : public GrabberBase
{
    Q_OBJECT
public:
    D3D10Grabber(QObject * parent, QList<QRgb> *grabResult, QList<GrabWidget *> *grabWidgets);
    ~D3D10Grabber();

    virtual void startGrabbing();
    virtual void stopGrabbing();
    virtual void isGrabbingStarted();
    virtual void setGrabInterval(int msec);

    void setFallbackGrabber(GrabberBase * grabber);

protected:
    virtual GrabResult _grab();

public slots:
    virtual void firstWidgetPositionChanged(QWidget * firstWidget);
    void init(void);

private slots:
    void infectCleanDxProcesses(void);

private:
    bool initIPC();
    void freeIPC();
    QRgb getColor(QRect &widgetRect);

    static HANDLE m_sharedMem;
    static HANDLE m_mutex;
    static PVOID m_memMap;
    GrabberBase * m_fallbackGrabber;
    D3D10GRABBER_SHARED_MEM_DESC m_memDesc;
    static UINT m_lastFrameId;
    static MONITORINFO m_monitorInfo;
    static QTimer * m_processesScanAndInfectTimer;
    static bool m_isInited;
    static ILibraryInjector * m_libraryInjector;
    static WCHAR m_hooksLibPath[300];
};

#endif
