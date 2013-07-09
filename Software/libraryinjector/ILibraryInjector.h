/*
 * LibraryInjector.hpp
 *
 *  Created on: 05.06.2012
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

#ifndef _ILIBRARYINJECTOR_HEADER
#define _ILIBRARYINJECTOR_HEADER


#include <windows.h>
#include <objbase.h>

#include <qglobal.h>

#if defined(LIBRARYINJECTOR_LIBRARY)
#  define LIBRARYINJECTORSHARED_EXPORT extern
#else
#  define LIBRARYINJECTORSHARED_EXPORT Q_DECL_IMPORT
#endif

// AppID                  = {04C77575-EB91-47b3-844B-45D0584D3853}
// IID_ILibraryInjector   = {029587AD-87F3-4623-941F-E37BF99A6DB2}
DEFINE_GUID(IID_ILibraryInjector  , 0x029587ad, 0x87f3, 0x4623, 0x94, 0x1f, 0xe3, 0x7b, 0xf9, 0x9a, 0x6d, 0xb2);

// CLSID_ILibraryInjector = {FC9D8F66-7B9A-47b7-8C5B-830BFF0E48C9}
DEFINE_GUID(CLSID_ILibraryInjector, 0xfc9d8f66, 0x7b9a, 0x47b7, 0x8c, 0x5b, 0x83, 0x0b, 0xff, 0x0e, 0x48, 0xc9);

#undef  INTERFACE
#define INTERFACE ILibraryInjector
DECLARE_INTERFACE_ (INTERFACE, IUnknown) {
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(Inject)(THIS_ DWORD ProcessId, LPWSTR ModulePath) PURE;
};


#endif
