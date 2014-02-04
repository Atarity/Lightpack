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

#ifndef WINUTILS_HPP
#define WINUTILS_HPP

#pragma once

#if !defined NOMINMAX
#define NOMINMAX
#endif

#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <QList>

namespace WinUtils
{
extern const WCHAR lightpackHooksDllName[];

BOOL SetPrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable);

BOOL AcquirePrivileges();

QList<DWORD> * getDxProcessesIDs(QList<DWORD> * processes, LPCWSTR wstrSystemRootPath);

PVOID BuildRestrictedSD(PSECURITY_DESCRIPTOR pSD);

// The following function frees memory allocated in the
// BuildRestrictedSD() function
VOID FreeRestrictedSD(PVOID ptr);
}

#endif // WINUTILS_HPP
