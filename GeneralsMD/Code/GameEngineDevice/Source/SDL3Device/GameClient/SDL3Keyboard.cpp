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

// FILE: SDL3Keyboard.cpp //////////////////////////////////////////////////////////////////////
// Created:   Stephan Vedder, March 2025
// Desc:      Device implementation of the keyboard interface on SDL3
//						using SDL3
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "Common/Debug.h"
#include "Common/Language.h"
#include "GameClient/KeyDefs.h"
#include "GameClient/Keyboard.h"
#include "SDL3Device/GameClient/SDL3Keyboard.h"
#include "GameClient/IMEManager.h"
#include "GameClient/GameWindowManager.h"

#include <SDL3/SDL.h>

// DEFINES ////////////////////////////////////////////////////////////////////////////////////////
enum { KEYBOARD_BUFFER_SIZE = 256 };

// PRIVATE DATA ///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
/** For debugging, prints the return code using direct input errors */
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/** create our interface to the direct input keybard */
//-------------------------------------------------------------------------------------------------
void SDL3Keyboard::openKeyboard( void )
{
  if(!SDL_InitSubSystem(SDL_INIT_EVENTS))
  {
    DEBUG_LOG(("ERROR - Could not initialize SDL3 Keyboard\n"));
    return;
  }
   
	DEBUG_LOG(( "OK - Keyboard initialized successfully.\n" ));

}  // end openKeyboard

//-------------------------------------------------------------------------------------------------
/** close the direct input keyboard */
//-------------------------------------------------------------------------------------------------
void SDL3Keyboard::closeKeyboard( void )
{
  SDL_QuitSubSystem(SDL_INIT_EVENTS);

	DEBUG_LOG(( "OK - Keyboard shutdown complete\n" ));

}  // end closeKeyboard

static KeyDefType ConvertSDLKey(SDL_Keycode keycode)
{
	if (keycode >= SDLK_A && keycode <= SDLK_Z)
	{
		return (KeyDefType)(KEY_A + (keycode - SDLK_A));
	}
	else if (keycode >= SDLK_0 && keycode <= SDLK_9)
	{
		return (KeyDefType)(KEY_0 + (keycode - SDLK_0));
	}

	switch (keycode)
	{
		case SDLK_RETURN:
			return KEY_ENTER;
		case SDLK_ESCAPE:
			return KEY_ESC;
		case SDLK_SPACE:
			return KEY_SPACE;
		case SDLK_BACKSPACE:
			return KEY_BACKSPACE;
		case SDLK_TAB:
			return KEY_TAB;
		case SDLK_LCTRL:
			return KEY_LCTRL;
		case SDLK_RCTRL:
			return KEY_RCTRL;
		case SDLK_LSHIFT:
			return KEY_LSHIFT;
		case SDLK_UP:
			return KEY_UP;
		case SDLK_DOWN:
			return KEY_DOWN;
		case SDLK_LEFT:
			return KEY_LEFT;
		case SDLK_RIGHT:
			return KEY_RIGHT;
		case SDLK_PERIOD:
			return KEY_PERIOD;
		case SDLK_PLUS:
			return KEY_KPPLUS;
		default:
			break;
	}

	return KEY_NONE;
}

//-------------------------------------------------------------------------------------------------
/** Get a single keyboard event from direct input */
//-------------------------------------------------------------------------------------------------
void SDL3Keyboard::getKey( KeyboardIO *key )
{
	static int errs = 0;
	int16_t num = 0;

	assert( key );
	key->sequence = 0;
	key->key = KEY_NONE;

	if(m_events.size() > 0)
	{
		SDL_Event event = m_events.front();
		m_events.erase(m_events.begin());
		if (TheIMEManager && TheIMEManager->getWindow() && event.type == SDL_EVENT_TEXT_INPUT)
		{
			// Note that special characters may need additional keycode translation above.
			const char* text = event.text.text;
			for (int i = 0; text[i] != '\0'; ++i)
			{
				WideChar wchar = static_cast<WideChar>(text[i]);
				TheWindowManager->winSendInputMsg(TheIMEManager->getWindow(), GWM_IME_CHAR, wchar, 0);
			}
		}
		else if(event.type == SDL_EVENT_KEY_DOWN)
		{
			key->key = ConvertSDLKey(event.key.key);
			key->state = KEY_STATE_DOWN;
		}
		else if(event.type == SDL_EVENT_KEY_UP)
		{
			key->key = ConvertSDLKey(event.key.key);
			key->state = KEY_STATE_UP;
		}
	}
	else
	{
		key->key = KEY_NONE;
	}

}  // end getKey

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SDL3Keyboard::SDL3Keyboard( void )
{
  SDL_Keymod mod = SDL_GetModState();
	if( mod & SDL_KMOD_CAPS )
	{
		m_modifiers |= KEY_STATE_CAPSLOCK;
	}
	else
	{
		m_modifiers &= ~KEY_STATE_CAPSLOCK;
	}

}  // end SDL3Keyboard

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SDL3Keyboard::~SDL3Keyboard( void )
{

	// close keyboard and release all resource
	closeKeyboard();

}  // end ~SDL3Keyboard

//-------------------------------------------------------------------------------------------------
/** initialize the keyboard */
//-------------------------------------------------------------------------------------------------
void SDL3Keyboard::init( void )
{

	// extending functionality
	Keyboard::init();

	// open the direct input keyboard
	openKeyboard();

}  // end init

//-------------------------------------------------------------------------------------------------
/** Reset keyboard system */
//-------------------------------------------------------------------------------------------------
void SDL3Keyboard::reset( void )
{
  SDL_ResetKeyboard();

	// extend functionality
	Keyboard::reset();

}  // end reset

//-------------------------------------------------------------------------------------------------
/** called once per frame to update the keyboard state */
//-------------------------------------------------------------------------------------------------
void SDL3Keyboard::update( void )
{

	// extending functionality
	Keyboard::update();

/*
	// make sure the keyboard buffer is flushed
	if( m_pKeyboardDevice )
	{
		DWORD items = INFINITE;

		m_pKeyboardDevice->GetDeviceData( sizeof( DIDEVICEOBJECTDATA ),
																			NULL, &items, 0 );

	}  // end if
*/

}  // end update

//-------------------------------------------------------------------------------------------------
/** Return TRUE if the caps lock key is down/hilighted */
//-------------------------------------------------------------------------------------------------
Bool SDL3Keyboard::getCapsState( void )
{
  SDL_Keymod mod = SDL_GetModState();
	return (mod & SDL_KMOD_CAPS) ? TRUE : FALSE;
}  // end getCapsState

void SDL3Keyboard::addSDLEvent(SDL_Event *ev)
{
	m_events.push_back(*ev);
	// Make sure to never have more than 256 events in the buffer
	if (m_events.size() >= 256)
	{
		m_events.erase(m_events.begin());
	}
}
