#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <d3d8.h>

#define DXVK_WSI_SDL2 1
#include <wsi/native_wsi.h>

#include "Win32Device/Common/Win32GameEngine.h"

GameEngine *CreateGameEngine( void )
{
	Win32GameEngine *engine;

	engine = NEW Win32GameEngine;
	//game engine may not have existed when app got focus so make sure it
	//knows about current focus state.
	engine->setIsActive(true);

	return engine;

}  // end CreateGameEngine

int main(int argc, char *argv[])
{
	// initialize the game engine using factory function
	TheGameEngine = CreateGameEngine();
	TheGameEngine->init(argc, argv);

	// run it
	TheGameEngine->execute();

	// since execute() returned, we are exiting the game
	delete TheGameEngine;
	TheGameEngine = NULL;
}