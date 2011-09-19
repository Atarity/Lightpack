/*
 * D3D9Grabber.hpp
 *
 *  Created on: 19.09.2011
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Timur Sattarov
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

#include<IGrabber.hpp>

#ifdef D3D9_GRAB_SUPPORT

#include<d3d9.h>

class D3D9Grabber : public IGrabber
{
public:
    D3D9Grabber();
    ~D3D9Grabber();
    virtual const char * getName();
    virtual void updateGrabScreenFromWidget( QWidget * widget ) {}
    virtual QList<QRgb> grabWidgetsColors(QList<GrabWidget *> &widgets);

private:
    LPDIRECT3D9 m_d3D;
    LPDIRECT3DDEVICE9 m_d3Device;
    LPDIRECT3DSURFACE9 m_surface;
    D3DDISPLAYMODE m_displayMode;
    D3DPRESENT_PARAMETERS m_presentParams;
    int m_bufLength;
    BYTE * m_buf;
    RECT m_rect;
private:
    BYTE * expandBuffer(BYTE * buf, int newLength);
    BYTE * getImageData(BYTE *, RECT &);
    RECT getEffectiveRect(QList<GrabWidget *> &widgets);
    int getBufLength(const RECT &rect);
    QRgb getColor(int x, int y, int width, int height);

};
#endif
