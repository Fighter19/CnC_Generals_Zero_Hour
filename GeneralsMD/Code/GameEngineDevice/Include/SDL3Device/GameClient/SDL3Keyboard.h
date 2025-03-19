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

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

// FILE: SDL3Keyboard.h ////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//                                                                          
//                       Westwood Studios Pacific.                          
//                                                                          
//                       Confidential Information					         
//                Copyright (C) 2001 - All Rights Reserved                  
//                                                                          
//-----------------------------------------------------------------------------
//
// Project:    RTS3
//
// File name:  SDL3Keyboard.h
//
// Created:    Stephan Vedder, March 2025
//
// Desc:       Device implementation of the keyboard interface on Win32
//						 using Microsoft Direct Input
//
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __SDL3KEYBOARD_H_
#define __SDL3KEYBOARD_H_

// SYSTEM INCLUDES ////////////////////////////////////////////////////////////

// USER INCLUDES //////////////////////////////////////////////////////////////
#include "GameClient/Keyboard.h"

// FORWARD REFERENCES /////////////////////////////////////////////////////////

// TYPE DEFINES ///////////////////////////////////////////////////////////////

// class SDL3Keyboard --------------------------------------------------
/** Class for interfacing with the keyboard using SDL3 as the
	* implementation */
//-----------------------------------------------------------------------------
class SDL3Keyboard : public Keyboard
{

public:

	SDL3Keyboard( void );
	virtual ~SDL3Keyboard( void );

	// extend methods from the base class
	virtual void init( void );		///< initialize the keyboard, extending init functionality
	virtual void reset( void );		///< Reset the keybaord system
	virtual void update( void );  ///< update call, extending update functionality
	virtual Bool getCapsState( void );		///< get state of caps lock key, return TRUE if down

protected:

	// extended methods from the base class
	virtual void getKey( KeyboardIO *key );  ///< get a single key event

	//-----------------------------------------------------------------------------------------------

	// new methods to this derived class
	void openKeyboard( void );  ///< create direct input keyboard
	void closeKeyboard( void );  ///< release direct input keyboard
 
};  // end class SDL3Keyboard

// INLINING ///////////////////////////////////////////////////////////////////

// EXTERNALS //////////////////////////////////////////////////////////////////

#endif // __SDL3KEYBOARD_H_

