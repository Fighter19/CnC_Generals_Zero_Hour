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

// FILE: SDL3Main.h ////////////////////////////////////////////////////////////
//
// Header for entry point for SDL3 application
//
// Author: Stephan Vedder, March 2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SDL3MAIN_H_
#define __SDL3MAIN_H_

// SYSTEM INCLUDES ////////////////////////////////////////////////////////////
#include <SDL3/SDL.h>

// USER INCLUDES //////////////////////////////////////////////////////////////
#include "SDL3Device/GameClient/SDL3Mouse.h"

// EXTERNAL ///////////////////////////////////////////////////////////////////
extern void* ApplicationHWnd;  ///< our application window handle
extern SDL3Mouse *TheSDL3Mouse;  ///< global for SDL3 mouse only!
extern SDL_Window* TheSDL3Window;

#endif  // end __WINMAIN_H_

