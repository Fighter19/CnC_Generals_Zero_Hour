// USER INCLUDES //////////////////////////////////////////////////////////////
#include "SDL3Main.h"
#include "Common/CopyProtection.h"
#include "Common/CriticalSection.h"
#include "Common/Debug.h"
#include "Common/GameEngine.h"
#include "Common/GameMemory.h"
#include "Common/GameSounds.h"
#include "Common/GlobalData.h"
#include "Common/MessageStream.h"
#include "Common/Registry.h"
#include "Common/StackDump.h"
#include "Common/Team.h"
#include "Common/Version.h"
#include "GameClient/GameClient.h"
#include "GameClient/IMEManager.h"
#include "GameClient/InGameUI.h"
#include "GameClient/Mouse.h"
#include "GameLogic/GameLogic.h" ///< @todo for demo, remove
#include "Lib/BaseType.h"
#include "SDL3Device/Common/SDL3GameEngine.h"
#include "SDL3Device/GameClient/SDL3Mouse.h"
// #include "BuildVersion.h"
// #include "GeneratedVersion.h"
#include <rts/profile.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <d3d8.h>

#define DXVK_WSI_SDL3 1
#include <wsi/native_wsi.h>

// GLOBALS
SDL_Window *TheSDL3Window = NULL;
Bool ApplicationIsWindowed = false;
SDL3Mouse *TheSDL3Mouse = NULL; ///< for the WndProc() only
DWORD TheMessageTime =
    0; ///< For getting the time that a message was posted from Windows.

const Char *g_strFile = "data\\Generals.str";
const Char *g_csfFile = "data\\%s\\Generals.csf";
const char *gAppPrefix = ""; /// So WB can have a different debug log file name.

static HANDLE GeneralsMutex = NULL;
#define GENERALS_GUID "685EAFF2-3216-4265-B047-251C5F4B82F3"
#define DEFAULT_XRESOLUTION 800
#define DEFAULT_YRESOLUTION 600

#include "GameNetwork/WOLBrowser/WebBrowser.h"
WebBrowser *TheWebBrowser;

AsciiString GetRegistryLanguage(void) {
#pragma message("Support for multiple languages not yet implemented")

  return "english";
}

UnsignedInt GetRegistryVersion(void) {
#pragma message("Version is not being encoded in registry on this platform")

  return 0;
}

GameEngine *CreateGameEngine(void) {
  SDL3GameEngine *engine;

  engine = NEW SDL3GameEngine;
  // game engine may not have existed when app got focus so make sure it
  // knows about current focus state.
  engine->setIsActive(true);

  return engine;

} // end CreateGameEngine

static Bool initializeAppWindows(Bool runWindowed) {
  DWORD windowStyle;
  Int startWidth = DEFAULT_XRESOLUTION, startHeight = DEFAULT_YRESOLUTION;

  if (runWindowed) {
    // Makes the normal debug 800x600 window center in the screen.
    startWidth = DEFAULT_XRESOLUTION;
    startHeight = DEFAULT_YRESOLUTION;
  }

  SDL_Window *window =
      SDL_CreateWindow("Game Window", startWidth, startHeight, 0);
  SDL_ShowWindow(window);
  // save our window handle for future use
  TheSDL3Window = window;

  return true; // success
}

int main(int argc, char *argv[]) {
#ifdef _PROFILE
  Profile::StartRange("init");
#endif

  // register windows class and create application window
  if (initializeAppWindows(ApplicationIsWindowed) == false)
    return 0;

  // initialize the game engine using factory function
  TheGameEngine = CreateGameEngine();
  TheGameEngine->init(argc, argv);

  // run it
  TheGameEngine->execute();

  // since execute() returned, we are exiting the game
  delete TheGameEngine;
  TheGameEngine = NULL;

	return 0;
}