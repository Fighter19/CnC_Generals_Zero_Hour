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

// FILE: SDL3Mouse.cpp ///////////////////////////////////////////////////////////////////////////
// Created:    Stephan Vedder, March 2025
// Desc:       Interface for the mouse using only the SDL3 events
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Common/Debug.h"
#include "GameClient/GameClient.h"
#include "SDL3Device/GameClient/SDL3Mouse.h"

#include <SDL3/SDL_events.h>

// EXTERN /////////////////////////////////////////////////////////////////////////////////////////


SDL_Cursor* cursorResources[Mouse::NUM_MOUSE_CURSORS][MAX_2D_CURSOR_DIRECTIONS];
///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS //////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDL_Cursor* SDL3Mouse::loadCursorFromFile(const char* file)
{
	// TODO: this should parse the ANI file and load the cursor correctly
	SDL_Surface *surface = SDL_LoadBMP(file);
	if (!surface) {
			// Failed to load surface
			DEBUG_LOG(("loadCursorFromFile: Failed to load ANI cursor [%s]", file));
			return NULL;
	}
	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, 0, 0);
	if (!cursor) {
			// Failed to create cursor
			DEBUG_LOG(("loadCursorFromFile: Failed to create SDL2 color cursor [%s]", file));
			return NULL;
	}

	return cursor;
}

//-------------------------------------------------------------------------------------------------
/** Get a mouse event from the buffer if available, we need to translate
	* from the windows message meanings to our own internal mouse 
	* structure */
//-------------------------------------------------------------------------------------------------
UnsignedByte SDL3Mouse::getMouseEvent( MouseIO *result, Bool flush )
{
	SDL_Event event;
	// if there is nothing here there is no event data to do
	if( m_eventBuffer[ m_nextGetIndex ].type == SDL_EVENT_FIRST )
		return MOUSE_NONE;

	// translate the SDL3 mouse message to our own system
	translateEvent( m_nextGetIndex, result );

	// remove this event from the buffer by setting msg to zero
	m_eventBuffer[ m_nextGetIndex ].type = SDL_EVENT_FIRST;

	//
	// our next get index will now be advanced to the next index, wrapping at
	// the mad
	//
	m_nextGetIndex++;
	if( m_nextGetIndex >= Mouse::NUM_MOUSE_EVENTS )
		m_nextGetIndex = 0;

	// got event OK and all done with this one
	return MOUSE_OK;

}  // end getMouseEvent

//-------------------------------------------------------------------------------------------------
/** Translate a win32 mouse event to our own event info */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::translateEvent( UnsignedInt eventIndex, MouseIO *result )
{
	SDL_EventType type = (SDL_EventType)m_eventBuffer[ eventIndex ].type;
	SDL_MouseButtonEvent&  mouseBtnEvent = m_eventBuffer[ eventIndex ].button;
	SDL_MouseMotionEvent&  mouseMotionEvent = m_eventBuffer[ eventIndex ].motion;
	SDL_MouseWheelEvent&  mouseWheelEvent = m_eventBuffer[ eventIndex ].wheel;
	UnsignedInt frame;

	//
	// get the current input frame from the client, if we don't have
	// a client (like in the GUI editor) we just use frame 1 so it
	// registers with the system
	//
	if( TheGameClient )
		frame = TheGameClient->getFrame();
	else
		frame = 1;

	// set these to defaults
	result->leftState = result->middleState = result->rightState = MBS_Up;
	result->leftFrame = result->middleFrame = result->rightFrame = 0;
	result->pos.x = result->pos.y = result->wheelPos = 0;

	// Time is the same for all events; from nanoseconds
	result->time = m_eventBuffer[ eventIndex ].common.timestamp / 1000000;
	
	switch( type )
	{

		// ------------------------------------------------------------------------
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			MouseButtonState buttonState = MBS_Down;

			if (mouseBtnEvent.clicks == 2) {
				buttonState = MBS_DoubleClick;
			}

			if (mouseBtnEvent.button == SDL_BUTTON_LEFT)
			{
				result->leftState = buttonState;
				result->leftFrame = frame;
			}
			else if (mouseBtnEvent.button == SDL_BUTTON_RIGHT)
			{
				result->rightState = buttonState;
				result->rightFrame = frame;
			}
			else if (mouseBtnEvent.button == SDL_BUTTON_MIDDLE)
			{
				result->middleState = buttonState;
				result->middleFrame = frame;
			}
			result->pos.x = mouseBtnEvent.x;
			result->pos.y = mouseBtnEvent.y;
			break;

		}
		// ------------------------------------------------------------------------
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			if (mouseBtnEvent.button == SDL_BUTTON_LEFT)
			{
				result->leftState = MBS_Up;
				result->leftFrame = frame;
			}
			else if (mouseBtnEvent.button == SDL_BUTTON_RIGHT)
			{
				result->rightState = MBS_Up;
				result->rightFrame = frame;
			}
			else if (mouseBtnEvent.button == SDL_BUTTON_MIDDLE)
			{
				result->middleState = MBS_Up;
				result->middleFrame = frame;
			}
			result->pos.x = mouseBtnEvent.x;
			result->pos.y = mouseBtnEvent.y;
			break;
		}  

		// ------------------------------------------------------------------------
		case SDL_EVENT_MOUSE_MOTION:
		{
			result->pos.x = mouseMotionEvent.x;
			result->pos.y = mouseMotionEvent.y;
			break;

		}  // end mouse move

		// ------------------------------------------------------------------------
		case SDL_EVENT_MOUSE_WHEEL:
		{	
			// Wheel delta to match to Windows behavior of the scroll wheel
			result->wheelPos =  mouseWheelEvent.y * 120;
			result->pos.x = mouseWheelEvent.mouse_x;
			result->pos.y = mouseWheelEvent.mouse_y;
			break;

		}  // end mouse wheel

		// ------------------------------------------------------------------------
		default:
		{

			DEBUG_CRASH(( "translateEvent: Unknown SDL3 mouse event [%i]\n", type ));
			return;

		}  // end default

	}  // end switch on message at event index in buffer

}  // end translateEvent

///////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SDL3Mouse::SDL3Mouse( void )
{

	// zero our event list
	memset( &m_eventBuffer, 0, sizeof( m_eventBuffer ) );

	m_nextFreeIndex = 0;
	m_nextGetIndex = 0;
	m_currentSdlCursor = NONE;
	for (Int i=0; i<NUM_MOUSE_CURSORS; i++)
		for (Int j=0; j<MAX_2D_CURSOR_DIRECTIONS; j++)
			cursorResources[i][j]=NULL;
	m_directionFrame=0; //points up.
	m_lostFocus = FALSE;
}  // end SDL3Mouse

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SDL3Mouse::~SDL3Mouse( void )
{

}  // end ~SDL3Mouse

//-------------------------------------------------------------------------------------------------
/** Initialize our device */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::init( void )
{

	// extending functionality
	Mouse::init();

	//
	// when we receive messages from a Windows message procedure, the mouse
	// moves report the current cursor position and not deltas, our mouse
	// needs to process those positions as absolute and not relative
	//
	m_inputMovesAbsolute = TRUE;

}  // end int

//-------------------------------------------------------------------------------------------------
/** Reset */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::reset( void )
{

	// extend
	Mouse::reset();

}  // end reset

//-------------------------------------------------------------------------------------------------
/** Update, called once per frame */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::update( void )
{

	// extend 
	Mouse::update();

}  // end update

//-------------------------------------------------------------------------------------------------
/** Add a SDL event to our input storage buffer */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::addSDLEvent( SDL_Event* ev )
{
	// check if this is a relevant event for us
	switch( ev->type )
	{
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		case SDL_EVENT_MOUSE_MOTION:
		case SDL_EVENT_MOUSE_WHEEL:
			break;
		default:
			DEBUG_LOG(("SDL3Mouse::addSDLEvent: No mouse event [%d]", ev->type));
			return;
	}

	//
	// we can only add this event if our next free index does not already
	// have an event in it, if it does ... our buffer is full and this input
	// event will be lost
	//
	if( m_eventBuffer[ m_nextFreeIndex ].type != SDL_EVENT_FIRST )
		return;

	// copy this event to our next free slot
	m_eventBuffer[ m_nextFreeIndex ] = *ev;


	// wrap index at max
	m_nextFreeIndex++;
	if( m_nextFreeIndex >= Mouse::NUM_MOUSE_EVENTS )
		m_nextFreeIndex = 0;

}  // end addSDLEvent


void SDL3Mouse::setVisibility(Bool visible)
{
	//Extend
	Mouse::setVisibility(visible);
	//Maybe need to set cursor to force hiding of some cursors.
	SDL3Mouse::setCursor(getMouseCursor());
}

/**Preload all the cursors we may need during the game.  This must be done before the D3D device
is created to avoid cursor corruption on buggy ATI Radeon cards. */
void SDL3Mouse::initCursorResources(void)
{
	for (Int cursor=FIRST_CURSOR; cursor<NUM_MOUSE_CURSORS; cursor++)
	{
		for (Int direction=0; direction<m_cursorInfo[cursor].numDirections; direction++)
		{	if (!cursorResources[cursor][direction] && !m_cursorInfo[cursor].textureName.isEmpty())
			{	//this cursor has never been loaded before.
				char resourcePath[256];
				//Check if this is a directional cursor
				if (m_cursorInfo[cursor].numDirections > 1)
					sprintf(resourcePath,"data\\cursors\\%s%d.ANI",m_cursorInfo[cursor].textureName.str(),direction);
				else
					sprintf(resourcePath,"data\\cursors\\%s.ANI",m_cursorInfo[cursor].textureName.str());

				cursorResources[cursor][direction]=loadCursorFromFile(resourcePath);
				DEBUG_ASSERTCRASH(cursorResources[cursor][direction], ("MissingCursor %s\n",resourcePath));
			}
		}
//		SetCursor(cursorResources[cursor][m_directionFrame]);
	}
}

//-------------------------------------------------------------------------------------------------
/** Super basic simplistic cursor */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::setCursor( MouseCursor cursor )
{
	// extend
	Mouse::setCursor( cursor );

	if (m_lostFocus)
		return;	//stop messing with mouse cursor if we don't have focus.

	if (cursor == NONE || !m_visible)
		SDL_SetCursor( NULL );
	else
	{
		SDL_SetCursor(cursorResources[cursor][m_directionFrame]);
	}  // end switch

	// save current cursor
	m_currentSdlCursor=m_currentCursor = cursor;
	
}  // end setCursor

//-------------------------------------------------------------------------------------------------
/** Capture the mouse to our application */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::capture( void )
{

//	SetCapture( ApplicationHWnd );

}  // end capture

//-------------------------------------------------------------------------------------------------
/** Release the mouse capture for our app window */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::releaseCapture( void )
{

//	ReleaseCapture();

}  // end releaseCapture








