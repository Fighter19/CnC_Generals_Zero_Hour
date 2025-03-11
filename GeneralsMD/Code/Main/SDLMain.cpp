#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <d3d8.h>

#define DXVK_WSI_SDL2 1
#include <wsi/native_wsi.h>

#include "UnixDevice/Common/UnixGameEngine.h"

HINSTANCE ApplicationHInstance = NULL;
CComModule _Module;

#include "GameNetwork/WOLBrowser/WebBrowser.h"
WebBrowser *TheWebBrowser;

AsciiString GetRegistryLanguage( void )
{
#pragma message("Support for multiple languages not yet implemented")

	return "english";
}

UnsignedInt GetRegistryVersion( void )
{
#pragma message("Version is not being encoded in registry on this platform")

	return 0;
}

GameEngine *CreateGameEngine( void )
{
	UnixGameEngine *engine;

	engine = NEW UnixGameEngine;
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