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
#include <SDL3_image/SDL_image.h>

#include <d3d8.h>

#define DXVK_WSI_SDL3 1
#include <wsi/native_wsi.h>

// GLOBALS
HINSTANCE ApplicationHInstance; ///< our application instance
HWND ApplicationHWnd = NULL;    ///< our application window handle
Bool ApplicationIsWindowed = false;
Bool ShowSplashScreen = true;
Win32Mouse *TheWin32Mouse = NULL; ///< for the WndProc() only
DWORD TheMessageTime = 0; ///< For getting the time that a message was posted from Windows.

const Char *g_strFile = "data\\Generals.str";
const Char *g_csfFile = "data\\%s\\Generals.csf";
const char *gAppPrefix = ""; /// So WB can have a different debug log file name.

static HANDLE GeneralsMutex = NULL;
#define GENERALS_GUID "685EAFF2-3216-4265-B047-251C5F4B82F3"
#define DEFAULT_XRESOLUTION 800
#define DEFAULT_YRESOLUTION 600

// SDL3 specific
#include "GameNetwork/WOLBrowser/WebBrowser.h"
WebBrowser *TheWebBrowser;
SDL_Window *TheSDL3Window = NULL;
CComModule _Module;
SDL_Window *SplashWindow = NULL;
SDL_Surface *SplashSurface = NULL;

static void postInit() {
  if (SplashWindow) {
    SDL_DestroyWindow(SplashWindow);
    SplashWindow = NULL;
  }
  if (TheSDL3Window)
    SDL_ShowWindow(TheSDL3Window);
}

char *nextParam(char *newSource, const char *seps)
{
  static char *source = NULL;
  if (newSource)
  {
    source = newSource;
  }
  if (!source)
  {
    return NULL;
  }

  // find first separator
  char *first = source;//strpbrk(source, seps);
  if (first)
  {
    // go past separator
    char *firstSep = strpbrk(first, seps);
    char firstChar[2] = {0,0};
    if (firstSep == first)
    {
      firstChar[0] = *first;
      while (*first == firstChar[0]) first++;
    }

    // find end
    char *end;
    if (firstChar[0])
      end = strpbrk(first, firstChar);
    else
      end = strpbrk(first, seps);

    // trim string & save next start pos
    if (end)
    {
      source = end+1;
      *end = 0;

      if (!*source)
        source = NULL;
    }
    else
    {
      source = NULL;
    }

    if (first && !*first)
      first = NULL;
  }

  return first;
}

static Bool initializeAppWindows(Bool runWindowed, Bool runSplash) {
  Int startWidth = DEFAULT_XRESOLUTION, startHeight = DEFAULT_YRESOLUTION;
  SDL_InitSubSystem(SDL_INIT_VIDEO);
  if (!SDL_Vulkan_LoadLibrary(nullptr)) {
    DEBUG_LOG(("Failed to load Vulkan library"));
    return false;
  }

  if (runWindowed) {
    // Makes the normal debug 800x600 window center in the screen.
    startWidth = DEFAULT_XRESOLUTION;
    startHeight = DEFAULT_YRESOLUTION;
  }

  TheSDL3Window = SDL_CreateWindow(
      "Command and Conquer Generals", startWidth, startHeight,
      SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
  if(!TheSDL3Window) {
    DEBUG_LOG(("Failed to create window"));
    return false;
  }

  SDL_IOStream* icoStream = SDL_IOFromFile("GeneralsZH.ico", "rb");
  if (icoStream) {
    SDL_Surface* icon = IMG_LoadICO_IO(icoStream);
    SDL_CloseIO(icoStream);
    if (icon) {
      SDL_SetWindowIcon(TheSDL3Window, icon);
      SDL_DestroySurface(icon);
    }
  }

  // save our window handle for future use
  ApplicationHWnd = TheSDL3Window;

  // Center the window
  SDL_SetWindowPosition(TheSDL3Window, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);

  setenv("DXVK_WSI_DRIVER", "SDL3", 1);

  if (runSplash) {
    SplashWindow =
      SDL_CreateWindow("Splash", SplashSurface->w, SplashSurface->h,
                       SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
  } else if (TheSDL3Window) {
    // If we aren't going to show the splash screen, go ahead and show the empty window
    SDL_ShowWindow(TheSDL3Window);
  }

  if (SplashWindow) {
    // Center the window
    SDL_SetWindowPosition(SplashWindow, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);

    SDL_Renderer* ren = SDL_CreateRenderer(SplashWindow, SDL_SOFTWARE_RENDERER);
    SDL_Texture* splashTexture = SDL_CreateTextureFromSurface(ren, SplashSurface);
    SDL_RenderTexture(ren, splashTexture, NULL, NULL);
    SDL_RenderPresent(ren);
    SDL_DestroySurface(SplashSurface);
    SplashSurface = NULL;
  }
  return true; // success
}

int main(int argc, char *argv[]) {
#ifdef _PROFILE
  Profile::StartRange("init");
#endif

  // This is similar to WinMain, where it looked up a few cmd line arguments before using the CommandLine module
  // Combine argv into a single string like lpCmdLine
  std::string cmdline;
  for (int i = 1; i < argc; ++i) {
    if (i > 1) cmdline += " ";
    if (strchr(argv[i], ' ')) {
      cmdline += "\"";
      cmdline += argv[i];
      cmdline += "\"";
    } else {
      cmdline += argv[i];
    }
  }

  char *cmdlineCStr = cmdline.data();
  char *token = nextParam(cmdlineCStr, "\" ");

  int local_argc = 1;
  char *local_argv[20];
  local_argv[0] = NULL;

  while (local_argc < 20 && token != NULL) {
    local_argv[local_argc++] = token;

    if (strcasecmp(token, "-nosplash") == 0)
      ShowSplashScreen = false;

    token = nextParam(NULL, "\" ");
  }

  if(ShowSplashScreen) {
    SplashSurface = SDL_LoadBMP("Install_Final.bmp");
  }

  // register windows class and create application window
  if (initializeAppWindows(ApplicationIsWindowed, ShowSplashScreen) == false)
    return 0;

  // start the log
  DEBUG_INIT(DEBUG_FLAGS_DEFAULT);
  initMemoryManager();

  // Set up version info
  TheVersion = NEW Version;

  GameMain(argc, argv);

  delete TheVersion;
  TheVersion = NULL;

#ifdef MEMORYPOOL_DEBUG
  TheMemoryPoolFactory->debugMemoryReport(
      REPORT_POOLINFO | REPORT_POOL_OVERFLOW | REPORT_SIMPLE_LEAKS, 0, 0);
#endif
#if defined(_DEBUG) || defined(_INTERNAL)
  TheMemoryPoolFactory->memoryPoolUsageReport("AAAMemStats");
#endif

  // close the log
  shutdownMemoryManager();
  DEBUG_SHUTDOWN();

  return 0;
}

GameEngine *CreateGameEngine(void) {
  SDL3GameEngine *engine;

  engine = NEW SDL3GameEngine;
  // game engine may not have existed when app got focus so make sure it
  // knows about current focus state.
  engine->setIsActive(true);
  engine->setPostInitCallback(&postInit);

  return engine;

} // end CreateGameEngine
