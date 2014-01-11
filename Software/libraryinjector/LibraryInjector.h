/*
 * LibraryInjector.h
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

#ifndef _LIBRARYINJECTOR_HEADER
#define _LIBRARYINJECTOR_HEADER

#include "ILibraryInjector.h"

int libraryInjectorInit(void);
int isLibraryInjectorActive(void);
void libraryInjectorShutdown(void);
void reportLog(WORD logType, LPCWSTR message, ...);

ILibraryInjector * allocLibraryInjector(void);
IClassFactory * allocClassFactory(void);

#endif // _LIBRARYINJECTOR_HEADER
