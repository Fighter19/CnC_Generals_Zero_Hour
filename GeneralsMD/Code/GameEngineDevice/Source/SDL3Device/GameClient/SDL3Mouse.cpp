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
#include "Common/File.h"
#include "Common/FileSystem.h"
#include "GameClient/GameClient.h"
#include "GameClient/Display.h"
#include "SDL3Device/GameClient/SDL3Mouse.h"

#include <SDL3/SDL_events.h>
#include <SDL3_image/SDL_image.h>

#include <array>

// a helper struct that holds the frames of an animated cursor
struct AnimatedCursor {
	std::array<SDL_Cursor*, MAX_2D_CURSOR_ANIM_FRAMES> m_frameCursors;
	std::array<SDL_Surface*, MAX_2D_CURSOR_ANIM_FRAMES> m_frameSurfaces;
	int m_currentFrame = 0;
	int m_frameCount = 0;
	int m_frameRate = 0; // the time a frame is displayed in 1/60th of a second

	~AnimatedCursor()
	{
		for (int i = 0; i < MAX_2D_CURSOR_ANIM_FRAMES; i++)
		{
			if (m_frameCursors[i])
			{
				SDL_DestroyCursor(m_frameCursors[i]);
				m_frameCursors[i] = nullptr;
			}
			if (m_frameSurfaces[i])
			{
				SDL_DestroySurface(m_frameSurfaces[i]);
				m_frameSurfaces[i] = nullptr;
			}
		}
	}
};

// EXTERN /////////////////////////////////////////////////////////////////////////////////////////
AnimatedCursor* cursorResources[Mouse::NUM_MOUSE_CURSORS][MAX_2D_CURSOR_DIRECTIONS];

///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS //////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::array<char, 4> FourCC;
constexpr FourCC riff_id = {'R', 'I', 'F', 'F'};
constexpr FourCC acon_id = {'A', 'C', 'O', 'N'};
constexpr FourCC anih_id = {'a', 'n', 'i', 'h'};
constexpr FourCC fram_id = {'f', 'r', 'a', 'm'};
constexpr FourCC icon_id = {'i', 'c', 'o', 'n'};
constexpr FourCC seq_id = {'s', 'e', 'q', ' '};
constexpr FourCC rate_id = {'r', 'a', 't', 'e'};
constexpr FourCC list_id = {'L', 'I', 'S', 'T'};

struct ANIHeader
{
	uint32_t size; // Should be 32 bytes (all fields below)
	uint32_t frames;
	uint32_t steps;
	uint32_t width;
	uint32_t height;
	uint32_t bitsPerPixel;
	uint32_t planes;
	uint32_t displayRate;
	uint32_t flags;
};

struct RIFFChunk
{
	FourCC id; // Should be 'RIFF' for the first 4 bytes
	uint32_t size; // Size of the file minus 8 bytes
	FourCC type; // Should be 'ACON' in the first chunk
};

static_assert(sizeof(ANIHeader) == 36, "ANIHeader size is not 36 bytes");

static RIFFChunk* getNextChunk(RIFFChunk* chunk)
{
	return (RIFFChunk*)((char*)chunk + sizeof(RIFFChunk) + chunk->size - 4);
}

static void* getChunkData(RIFFChunk* chunk)
{
	// Type is also part of the chunk, but also data, remove it from the size
	return (char*)chunk + sizeof(RIFFChunk) - 4;
}

AnimatedCursor* SDL3Mouse::loadCursorFromFile(const char* filepath)
{
	File* file = TheFileSystem->openFile(filepath);
	if (!file)
	{
		DEBUG_LOG(("loadCursorFromFile: Failed to open ANI cursor [%s]", filepath));
		return NULL;
	}

	// Read entire file and close it
	Int size  = file->size();
	char* file_buffer = file->readEntireAndClose();

	if (!file_buffer)
	{
		DEBUG_LOG(("loadCursorFromFile: Failed to read ANI cursor [%s]", filepath));
		file->close();
		return NULL;
	}

	RIFFChunk *riff_header = (RIFFChunk*)file_buffer;
	if (riff_header->id != riff_id)
	{
		DEBUG_LOG(("loadCursorFromFile: Not a RIFF file"));
		delete[] file_buffer;
		return NULL;
	}

	if(riff_header->type != acon_id) {
		DEBUG_LOG(("loadCursorFromFile: Not an animated cursor file"));
		return NULL;
	}
	
	DEBUG_LOG(("loadCursorFromFile: loading %s", filepath));
	AnimatedCursor* cursor = new AnimatedCursor();

	RIFFChunk* chunk = (RIFFChunk*)((char*)file_buffer + sizeof(RIFFChunk));

	while (chunk != NULL && (char *)chunk < file_buffer + size)
	{
		if (chunk->id == anih_id)
		{
			if (chunk->size != sizeof(ANIHeader))
			{
				DEBUG_LOG(("loadCursorFromFile: Invalid ANI header size"));
				return NULL;
			}

			ANIHeader *ani_header = (ANIHeader*)getChunkData(chunk);

			cursor->m_frameCount = ani_header->frames;
			cursor->m_frameRate = ani_header->displayRate;
		}
		else if (chunk->id == list_id && chunk->type == fram_id)
		{
			int frame_index = 0;
			size_t frame_offset = 0;

			RIFFChunk *frame = (RIFFChunk*)((char *)chunk + sizeof(RIFFChunk));
			while (frame != NULL && (char *)frame < file_buffer + size)
			{
				if (frame->id == icon_id)
				{
					const void *frame_buffer = getChunkData(frame);
					SDL_IOStream *io_stream = SDL_IOFromConstMem(frame_buffer, frame->size);
					SDL_Surface *surface = cursor->m_frameSurfaces[frame_index] = IMG_LoadTyped_IO(io_stream, true, "ico");

					if (!surface)
					{
						DEBUG_LOG(("loadCursorFromFile: Failed to load frame"));
						return NULL;
					}

					// Allow specifying the hot spot via properties on the surface
					SDL_PropertiesID props = SDL_GetSurfaceProperties(surface);
					int hot_spot_x = (int)SDL_GetNumberProperty(props, SDL_PROP_SURFACE_HOTSPOT_X_NUMBER, 0);
					int hot_spot_y = (int)SDL_GetNumberProperty(props, SDL_PROP_SURFACE_HOTSPOT_Y_NUMBER, 0);

					cursor->m_frameCursors[frame_index++] = SDL_CreateColorCursor(surface, hot_spot_x, hot_spot_y);
				}

				if (frame_index >= MAX_2D_CURSOR_ANIM_FRAMES)
				{
					DEBUG_LOG(("loadCursorFromFile: Too many frames"));
					break;
				}

				frame = getNextChunk(frame);
			}
			break;
		}
		else
		{
			DEBUG_LOG(("loadCursorFromFile: Unhandled chunk"));
		}
		chunk = getNextChunk(chunk);
	}

#ifdef _DEBUG
	size_t loaded_frames = 0;
	for (int i = 0; i < MAX_2D_CURSOR_ANIM_FRAMES; i++)
	{
		if (cursor->m_frameCursors[i])
			loaded_frames++;
	}

	DEBUG_ASSERTCRASH(loaded_frames == cursor->m_frameCount, ("Loaded frames do not match header"));
#endif

	delete[] file_buffer;
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

void SDL3Mouse::scaleMouseCoordinates(int rawX, int rawY, Uint32 windowID, int& scaledX, int& scaledY)
{
	SDL_Window* window = SDL_GetWindowFromID(windowID);
	if (!window) {
		scaledX = rawX;
		scaledY = rawY;
		return;
	}

	int windowWidth = 0, windowHeight = 0;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	int internalWidth = TheDisplay->getWidth();
	int internalHeight = TheDisplay->getHeight();

	float factorX = static_cast<float>(internalWidth) / windowWidth;
	float factorY = static_cast<float>(internalHeight) / windowHeight;

	scaledX = static_cast<int>(rawX * factorX);
	scaledY = static_cast<int>(rawY * factorY);
}

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
	int scaledX = 0, scaledY = 0;
	
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

			scaleMouseCoordinates(mouseBtnEvent.x, mouseBtnEvent.y, mouseBtnEvent.windowID, scaledX, scaledY);
			result->pos.x = scaledX;
			result->pos.y = scaledY;

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

			scaleMouseCoordinates(mouseBtnEvent.x, mouseBtnEvent.y, mouseBtnEvent.windowID, scaledX, scaledY);
			result->pos.x = scaledX;
			result->pos.y = scaledY;

			break;
		}  

		// ------------------------------------------------------------------------
		case SDL_EVENT_MOUSE_MOTION:
		{
			scaleMouseCoordinates(mouseMotionEvent.x, mouseMotionEvent.y, mouseMotionEvent.windowID, scaledX, scaledY);
			result->pos.x = scaledX;
			result->pos.y = scaledY;
			break;

		}  // end mouse move

		// ------------------------------------------------------------------------
		case SDL_EVENT_MOUSE_WHEEL:
		{	
			// Wheel delta to match to Windows behavior of the scroll wheel
			result->wheelPos =  mouseWheelEvent.y * 120;

			scaleMouseCoordinates(mouseWheelEvent.mouse_x, mouseWheelEvent.mouse_y, mouseWheelEvent.windowID, scaledX, scaledY);
			result->pos.x = scaledX;
			result->pos.y = scaledY;

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
				cursorResources[i][j]= NULL;
	m_directionFrame=0; //points up.
	m_lostFocus = FALSE;
}  // end SDL3Mouse

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SDL3Mouse::~SDL3Mouse( void )
{
	for (Int i = 0; i < NUM_MOUSE_CURSORS; i++)
	{
		for (Int j = 0; j < MAX_2D_CURSOR_DIRECTIONS; j++)
		{
			if (cursorResources[i][j])
			{
				delete cursorResources[i][j];
				cursorResources[i][j] = NULL;
			}
		}
	}
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
					sprintf(resourcePath,"Data/Cursors/%s%d.ani",m_cursorInfo[cursor].textureName.str(),direction);
				else
					sprintf(resourcePath,"Data/Cursors/%s.ani",m_cursorInfo[cursor].textureName.str());

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

	bool bUseDefaultCursor = false;
	if (cursor == NONE || !m_visible)
	{
		bUseDefaultCursor = true;
	}
	else
	{
		AnimatedCursor* currentCursor = cursorResources[cursor][m_directionFrame];
		if (currentCursor)
		{
			// The game runs at 30FPS, the frame rate within the metadata is for 60FPS
			size_t index = m_inputFrame * 2 / currentCursor->m_frameRate;
			SDL_SetCursor(currentCursor->m_frameCursors[index % currentCursor->m_frameCount]);
		}
		else
		{
			bUseDefaultCursor = true;
		}
	}  // end switch

	if (bUseDefaultCursor)
	{
		if (cursorResources[NORMAL][0])
		{
			SDL_SetCursor(cursorResources[NORMAL][0]->m_frameCursors[0]);
		}
		else
		{
			// Fallback to SDL's default cursor in case of failure
			// This is to avoid crashing in case of case sensitivity issues
			SDL_SetCursor(SDL_GetDefaultCursor());
		}
	}

	// save current cursor
	m_currentSdlCursor=m_currentCursor = cursor;
	
}  // end setCursor

//-------------------------------------------------------------------------------------------------
/** Capture the mouse to our application */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::capture( void )
{
	SDL_CaptureMouse(true);

}  // end capture

//-------------------------------------------------------------------------------------------------
/** Release the mouse capture for our app window */
//-------------------------------------------------------------------------------------------------
void SDL3Mouse::releaseCapture( void )
{

	SDL_CaptureMouse(false);

}  // end releaseCapture








