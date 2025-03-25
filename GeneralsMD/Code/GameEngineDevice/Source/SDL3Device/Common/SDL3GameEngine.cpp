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

// FILE: W3DGameEngine.cpp ////////////////////////////////////////////////////////////////////////
// Author: Colin Day, April 2001
// Description:
//   Implementation of the Win32 game engine, this is the highest level of 
//   the game application, it creates all the devices we will use for the game
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SDL3Device/Common/SDL3GameEngine.h"
#include "SDL3Device/GameClient/SDL3Mouse.h"
#include "SDL3Device/GameClient/SDL3Keyboard.h"
#include "Common/PerfTimer.h"

#include "GameNetwork/LANAPICallbacks.h"

#include <SDL3/SDL.h>

extern DWORD TheMessageTime;

//-------------------------------------------------------------------------------------------------
/** Constructor for SDL3GameEngine */
//-------------------------------------------------------------------------------------------------
SDL3GameEngine::SDL3GameEngine()
{
}

//-------------------------------------------------------------------------------------------------
/** Destructor for SDL3GameEngine */
//-------------------------------------------------------------------------------------------------
SDL3GameEngine::~SDL3GameEngine()
{
}


//-------------------------------------------------------------------------------------------------
/** Initialize the game engine */
//-------------------------------------------------------------------------------------------------
void SDL3GameEngine::init( void )
{

	// extending functionality
	GameEngine::init();

	// notify SDLMain that we are done initializing
	if(m_postInitCallback)
	{
		m_postInitCallback();
	}
}  // end init

void SDL3GameEngine::init( int argc, char** argv )
{
	// extending functionality
	GameEngine::init(argc, argv);

	// notify SDLMain that we are done initializing
	if(m_postInitCallback)
	{
		m_postInitCallback();
	}
}  // end init

//-------------------------------------------------------------------------------------------------
/** Reset the system */
//-------------------------------------------------------------------------------------------------
void SDL3GameEngine::reset( void )
{

	// extending functionality
	GameEngine::reset();

}  // end reset

//-------------------------------------------------------------------------------------------------
/** Update the game engine by updating the GameClient and 
	* GameLogic singletons. */
//-------------------------------------------------------------------------------------------------
void SDL3GameEngine::update( void )
{
	// call the engine normal update
	GameEngine::update();

	extern SDL_Window* TheSDL3Window;
	if (TheSDL3Window && SDL_GetWindowFlags(TheSDL3Window) & SDL_WINDOW_MINIMIZED) {
		while (TheSDL3Window && SDL_GetWindowFlags(TheSDL3Window) & SDL_WINDOW_MINIMIZED) {
			// We are alt-tabbed out here.  Sleep a bit, & process windows
			// so that we can become un-alt-tabbed out.
      SDL_Delay(5);
			serviceWindowsOS();

			if (TheLAN != NULL) {
				// BGC - need to update TheLAN so we can process and respond to other
				// people's messages who may not be alt-tabbed out like we are.
				TheLAN->setIsActive(isActive());
				TheLAN->update();
			}

			// If we are running a multiplayer game, keep running the logic.
			// There is code in the client to skip client redraw if we are 
			// iconic.  jba.
			if (TheGameEngine->getQuitting() || TheGameLogic->isInInternetGame() || TheGameLogic->isInLanGame()) {
				break; // keep running.
			}
		}
    
    // When we are alt-tabbed out... the MilesAudioManager seems to go into a coma sometimes
    // and not regain focus properly when we come back. This seems to wake it up nicely.
    AudioAffect aa = (AudioAffect)0x10;
		TheAudio->setVolume(TheAudio->getVolume( aa ), aa );

	}

	// allow windows to perform regular windows maintenance stuff like msgs
	serviceWindowsOS();

}  // end update

//-------------------------------------------------------------------------------------------------
/** This function may be called from within this application to let
  * Microsoft Windows do its message processing and dispatching.  Presumeably
	* we would call this at least once each time around the game loop to keep
	* Windows services from backing up */
//-------------------------------------------------------------------------------------------------
void SDL3GameEngine::serviceWindowsOS( void )
{
	// Let SDL fill it's event queue
	SDL_PumpEvents();

	SDL_Event event;
	while(SDL_PollEvent(&event)) {
	  TheMessageTime = event.common.timestamp;

		SDL3Mouse* mouse = dynamic_cast<SDL3Mouse*>(TheMouse);
		SDL3Keyboard* keyboard = dynamic_cast<SDL3Keyboard*>(TheKeyboard);
		
		switch(event.type) {
			case SDL_EVENT_MOUSE_BUTTON_UP:
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			case SDL_EVENT_MOUSE_MOTION:
			case SDL_EVENT_MOUSE_WHEEL:
				if(mouse) {
					mouse->addSDLEvent(&event);
				}
				break;
			case SDL_EVENT_KEY_DOWN:
			case SDL_EVENT_KEY_UP:
			case SDL_EVENT_KEYBOARD_ADDED:
			case SDL_EVENT_KEYBOARD_REMOVED:
			case SDL_EVENT_KEYMAP_CHANGED:
				if(keyboard) {
					keyboard->addSDLEvent(&event);
				}
				break;
			// default:
			// 	SDL_PushEvent(&event);
				break;
		}

		TheMessageTime = 0;
	}
}  // end ServiceWindowsOS

