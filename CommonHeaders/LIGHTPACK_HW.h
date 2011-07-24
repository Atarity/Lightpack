#/*
# * LIGHTPACK_HW.h
# *
# *  Created on: 15.04.2010
# *      Author: Mike Shatohin (brunql)
# *     Project: Lightpack
# *
# *  Lightpack is a content-appropriate ambient lighting system for any computer
# *
# *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
# *
# *  Lightpack is free software: you can redistribute it and/or modify
# *  it under the terms of the GNU General Public License as published by
# *  the Free Software Foundation, either version 2 of the License, or
# *  (at your option) any later version.
# *
# *  Lightpack is distributed in the hope that it will be useful,
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# *  GNU General Public License for more details.
# *
# *  You should have received a copy of the GNU General Public License
# *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
# *
# */

#ifndef LIGHTPACK_HW_H_INCLUDED
#define LIGHTPACK_HW_H_INCLUDED

#/*
# *  Define Lightpack major hardware version TWO times
# *  
# *  Example for hw5.x: 
# *     1) LIGHTPACK_HW = 4
# *     2) #define LIGHTPACK_HW  4
# */

# /* FIRST for firmware make preprocessor */
# /*
           LIGHTPACK_HW = 4
# */

#if !defined( LIGHTPACK_HW )

# /* SECOND for C/C++ preprocessor */

#   define LIGHTPACK_HW   5

#endif /* !defined( LIGHTPACK_HW ) */

#endif /* LIGHTPACK_HW_H_INCLUDED */

