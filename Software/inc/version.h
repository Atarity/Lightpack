/*
 * version.h
 *
 *  Created on: 27.07.2010
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2010, 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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


#ifndef VERSION_H
#define VERSION_H

// Current version of Lightpack software
#define VERSION_STR     "4.1.0beta1"

// Resource defines using in Lightpack.rc
#define VER_FILEVERSION             4,1,0,1
#define VER_PRODUCTVERSION          VER_FILEVERSION

#define VER_FILEVERSION_STR         VERSION_STR "\0"
#define VER_PRODUCTVERSION_STR      VER_FILEVERSION_STR

#define VER_PRODUCTNAME_STR         "Lightpack"
#define VER_COMPANYNAME_STR         "Lightpack"
#define VER_FILEDESCRIPTION_STR     "lightpack.googlecode.com"
#define VER_INTERNALNAME_STR        "Lightpack"
#define VER_LEGALCOPYRIGHT_STR      "Copyleft © 2011 Lightpack"
#define VER_ORIGINALFILENAME_STR    "Lightpack_sw" VERSION_STR ".exe"


#endif // VERSION_H
