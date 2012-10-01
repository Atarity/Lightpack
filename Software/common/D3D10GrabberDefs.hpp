/*
 * D3D10GrabberDefs.hpp
 *
 *  Created on: 01.06.2012
 *      Author: Timur Sattarov
 *     Project: Lightpack
 *
 *  Copyright (c) 2012 Timur Sattarov
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
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

#define D3D10GRABBER_SHARED_MEM_NAME L"Lightpack.D3D10Grabber.SharedMem"
#define MB (1024*1024)
#define D3D10GRABBER_SHARED_MEM_SIZE (50*MB)
#define D3D10GRABBER_MUTEX_MEM_NAME L"Lightpack.D3D10Grabber.Mutex"
#define D3D10GRABBER_FRAMEGRABBED_EVENT_NAME L"Lightpack.D3D10Grabber.Event.FrameGrabbed"
#define D3D10GRABBER_PIPE_NAME L"\\\\.\\pipe\\Lightpack.D3D10Grabber"

#define D3D10GRABBER_BLANK_FRAME_ID 0xffffffff

struct D3D10GRABBER_SHARED_MEM_DESC {
    UINT dxgiPresentFuncOffset;
    UINT width;
    UINT height;
    UINT rowPitch;
    UINT frameId;
    UINT verboseDebug;
};

enum D3D10GRABBER_PIPE_MSG_TYPE {
    D3D10GRABBER_PIPE_MSG_QUIT,
    D3D10GRABBER_PIPE_MSG_FRAMEGRABBED
};

struct D3D10GRABBER_PIPE_MSG {
    D3D10GRABBER_PIPE_MSG_TYPE  msgType;
    UINT                        from;
    UINT                        to;
};

#define D3D10GRABBER_SHARED_MEM_DATA_SIZE (D3D10GRABBER_SHARED_MEM_SIZE - sizeof(D3D10GRABBER_SHARED_MEM_DESC))
