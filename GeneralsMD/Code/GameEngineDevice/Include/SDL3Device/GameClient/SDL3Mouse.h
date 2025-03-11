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
//  (c) 2001-2003 Electronic Arts Inc.
//  //
//																																						//
////////////////////////////////////////////////////////////////////////////////

// FILE: SDL3Mouse.h /////////////////////////////////////////////////////////
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
// File name:  SDL3Mouse.h
//
// Created:    Stephan Vedder, March 2025
//
// Desc:       Interface for the mouse using only the SDL3 messages
//
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __SDL3MOUSE_H_
#define __SDL3MOUSE_H_

// SYSTEM INCLUDES ////////////////////////////////////////////////////////////

// USER INCLUDES //////////////////////////////////////////////////////////////
#include "GameClient/Mouse.h"

// FORWARD REFERENCES /////////////////////////////////////////////////////////
struct SDLEvent;

// TYPE DEFINES ///////////////////////////////////////////////////////////////

// SDL3Mouse -----------------------------------------------------------------
/** Mouse interface for when using only the Win32 messages */
//-----------------------------------------------------------------------------
class SDL3Mouse : public Mouse {

public:
  SDL3Mouse(void);
  virtual ~SDL3Mouse(void);

  virtual void
  init(void); ///< init mouse, extend this functionality, do not replace
  virtual void reset(void);  ///< reset the system
  virtual void update(void); ///< update
  virtual void
  initCursorResources(void); ///< load windows resources needed for 2d cursors.

  virtual void setCursor(MouseCursor cursor); ///< set mouse cursor
  virtual void capture(void);                 ///< capture the mouse
  virtual void releaseCapture(void);          ///< release mouse capture

  virtual void setVisibility(Bool visible);

  /// add an event from a win32 window procedure
  void addSDLEvent(SDLEvent *ev);
  void lostFocus(Bool state) { m_lostFocus = state; }

protected:
  /// get the next event available in the buffer
  virtual UnsignedByte getMouseEvent(MouseIO *result, Bool flush);

  /// translate a win32 mouse event to our own info
  void translateEvent(UnsignedInt eventIndex, MouseIO *result);

  struct SDL3MouseEvent {};
  /// this is our buffer of events that we receive via a WndProc message
  SDL3MouseEvent m_eventBuffer[Mouse::NUM_MOUSE_EVENTS];
  UnsignedInt m_nextFreeIndex; ///< insert new events at this index
  UnsignedInt m_nextGetIndex;  /** events retrieved through getMouseEvent
                               will come from this index, then it will be
                               incremented to the next index */
  MouseCursor
      m_currentWin32Cursor; ///< keep track of last cursor image sent to D3D.
  Int m_directionFrame; ///< current frame of directional cursor (frome 0 points
                        ///< up).
  Bool m_lostFocus; ///< flag if window has lost focues and mouse should stop
                    ///< being updated.
};

// INLINING ///////////////////////////////////////////////////////////////////

// EXTERNALS //////////////////////////////////////////////////////////////////

#endif // __SDL3MOUSE_H_
