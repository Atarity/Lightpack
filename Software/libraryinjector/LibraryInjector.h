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
#include"initguid.h"
#include"ILibraryInjector.h"

typedef struct {
    ILibraryInjectorVtbl *lpVtbl;
    volatile LONG refCount;
} LibraryInjector;

typedef struct {
    IClassFactoryVtbl *lpVtbl;
    volatile LONG refCount;
} ClassFactory;


static HRESULT STDMETHODCALLTYPE LibraryInjector_QueryInterface(LibraryInjector * this, REFIID interfaceGuid, void **ppv);
static ULONG STDMETHODCALLTYPE LibraryInjector_AddRef(LibraryInjector * this);
static ULONG STDMETHODCALLTYPE LibraryInjector_Release(LibraryInjector * this);
static HRESULT STDMETHODCALLTYPE LibraryInjector_Inject(LibraryInjector * this, DWORD Process, LPWSTR ModulePath);


static HRESULT STDMETHODCALLTYPE ClassFactory_QueryInterface(ClassFactory * this, REFIID factoryGuid, void **ppv);
static ULONG STDMETHODCALLTYPE ClassFactory_AddRef(ClassFactory * this);
static ULONG STDMETHODCALLTYPE ClassFactory_Release(ClassFactory * this);
static HRESULT STDMETHODCALLTYPE ClassFactory_CreateInstance(ClassFactory * this, IUnknown *punkOuter, REFIID vTableGuid, void **objHandle);
static HRESULT STDMETHODCALLTYPE ClassFactory_LockServer(ClassFactory * this, BOOL flock);

static const ILibraryInjectorVtbl libraryInjectorVtbl = {LibraryInjector_QueryInterface,
                                            LibraryInjector_AddRef,
                                            LibraryInjector_Release,
                                            LibraryInjector_Inject};

static const IClassFactoryVtbl classFactoryVtbl = {ClassFactory_QueryInterface,
                                      ClassFactory_AddRef,
                                      ClassFactory_Release,
                                      ClassFactory_CreateInstance,
                                      ClassFactory_LockServer};
