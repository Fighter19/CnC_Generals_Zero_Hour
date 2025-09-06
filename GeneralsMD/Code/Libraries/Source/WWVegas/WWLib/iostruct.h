/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : WWLib                                                        *
 *                                                                                             *
 *                     $Archive:: /G/wwlib/iostruct.h                                         $*
 *                                                                                             *
 *                       Author:: Greg Hjelstrom                                               *
 *                                                                                             *
 *                     $Modtime:: 4/02/99 11:59a                                              $*
 *                                                                                             *
 *                    $Revision:: 3                                                           $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#if defined(_MSC_VER)
#pragma once
#endif

#ifndef IOSTRUCT_H
#define IOSTRUCT_H

#ifndef BITTYPE_H
#include "bittype.h"
#endif

#include "../WWMath/CustomFloat.h"

/*
** Some useful structures for writing/writing (safe from changes).
** The chunk IO classes contain code for reading and writing these.
*/
struct IOVector2Struct
{
	CustomFloat		X;
	CustomFloat		Y;
};

struct IOVector3Struct
{
	CustomFloat		X;							// X,Y,Z coordinates
	CustomFloat		Y;
	CustomFloat		Z;
};

static_assert(sizeof(IOVector3Struct) == 12, "IOVector3Struct size is not 12 bytes");

struct IOVector4Struct
{
	CustomFloat		X;
	CustomFloat		Y;
	CustomFloat		Z;
	CustomFloat		W;
};

struct IOQuaternionStruct
{
	CustomFloat		Q[4];
};



#endif

