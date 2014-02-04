/*
 * WinUtils.hpp
 *
 *  Created on: 25.07.11
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Timur Sattarov, Mike Shatohin
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

#ifndef WINDXUTILS_HPP
#define WINDXUTILS_HPP

#pragma once

#if !defined NOMINMAX
#define NOMINMAX
#endif

#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace WinUtils
{
/*!
  We are not allowed to create DXGIFactory inside of Dll, so determine SwapChain->Present vtbl location here.
  To determine location we need to create SwapChain instance and dereference pointer.
 \param hWnd window handle to bind SwapChain to
 \return UINT
*/
UINT GetDxgiPresentOffset(HWND hWnd);
UINT GetD3D9PresentOffset(HWND hWnd);
UINT GetD3D9SCPresentOffset(HWND hWnd);
}

#endif // WINDXUTILS_HPP
