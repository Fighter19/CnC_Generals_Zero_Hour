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
// Adapted from WinMain.cpp
// Created:    Patrick Zacharias, September 2025
// Desc:       Simple test to initialize TheDisplay and draw a splash screen
///////////////////////////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES ////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <ole2.h>
#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3_image/SDL_image.h>

// USER INCLUDES //////////////////////////////////////////////////////////////
#include "Lib/BaseType.h"
#include "Common/CopyProtection.h"
#include "Common/CriticalSection.h"
#include "Common/GlobalData.h"
#include "Common/GameEngine.h"
#include "Common/GameSounds.h"
#include "Common/Debug.h"
#include "Common/GameMemory.h"
// #include "Common/SafeDisc/CdaPfn.h"
#include "Common/StackDump.h"
#include "Common/MessageStream.h"
#include "Common/Registry.h"
#include "Common/Team.h"
#include "GameClient/InGameUI.h"
#include "GameClient/GameClient.h"
#include "GameLogic/GameLogic.h" ///< @todo for demo, remove
#include "GameClient/Mouse.h"
#include "GameClient/IMEManager.h"
#include "GameClient/Image.h"
#include "Common/Version.h"
#include "Common/XferCRC.h"

#include "StdDevice/Common/StdBIGFileSystem.h"
#include "StdDevice/Common/StdLocalFileSystem.h"

#include "W3DDevice/GameClient/W3DDisplay.h"

#include "Common/CommandLine.h"
#include "Common/GameLOD.h"

#include "W3DDevice/GameClient/W3DDisplayStringManager.h"
#include "W3DDevice/GameClient/W3DGameFont.h"

#include "WW3D2/ww3d.h"

#ifndef _WIN32
#include "GameNetwork/WOLBrowser/WebBrowser.h"
WebBrowser *TheWebBrowser;
CComModule _Module;
#endif

// GLOBALS ////////////////////////////////////////////////////////////////////
HINSTANCE ApplicationHInstance = NULL; ///< our application instance
HWND ApplicationHWnd = NULL;           ///< our application window handle
Bool ApplicationIsWindowed = false;
DWORD TheMessageTime = 0; ///< For getting the time that a message was posted from Windows.

#define DEFAULT_XRESOLUTION 800
#define DEFAULT_YRESOLUTION 600

const Char *g_strFile = "data\\Generals.str";
const Char *g_csfFile = "data\\%s\\Generals.csf";
const char *gAppPrefix = ""; /// So WB can have a different debug log file name.

SDL_Window *TheSDL3Window = NULL;
SDL_Window *TheSDL3WindowVulkan = NULL; ///< Vulkan window, if used

// Necessary to allow memory managers and such to have useful critical sections
static CriticalSection critSec1, critSec2, critSec3, critSec4, critSec5;

template <class SUBSYSTEM>
void initSubsystem(SUBSYSTEM *&sysref, AsciiString name, SUBSYSTEM *sys, Xfer *pXfer, const char *path1 = NULL,
                   const char *path2 = NULL, const char *dirpath = NULL)
{
  sysref = sys;
  TheSubsystemList->initSubsystem(sys, path1, path2, dirpath, pXfer, name);
}

static Bool initializeAppWindows(Bool runWindowed)
{
  Int startWidth = DEFAULT_XRESOLUTION, startHeight = DEFAULT_YRESOLUTION;
  SDL_InitSubSystem(SDL_INIT_VIDEO);
  if (!SDL_Vulkan_LoadLibrary(nullptr))
  {
    DEBUG_LOG(("Failed to load Vulkan library"));
    return false;
  }

  if (runWindowed)
  {
    // Makes the normal debug 800x600 window center in the screen.
    startWidth = DEFAULT_XRESOLUTION;
    startHeight = DEFAULT_YRESOLUTION;
  }

  TheSDL3Window = SDL_CreateWindow(
      "Command and Conquer Generals", startWidth, startHeight,
      SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
  if (!TheSDL3Window)
  {
    DEBUG_LOG(("Failed to create window"));
    return false;
  }

  TheSDL3WindowVulkan = SDL_CreateWindow(
      "Command and Conquer Generals Vulkan", startWidth, startHeight,
      SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
  if (!TheSDL3WindowVulkan)
  {
    DEBUG_LOG(("Failed to create Vulkan window"));
    SDL_DestroyWindow(TheSDL3Window);
    TheSDL3Window = NULL;
    return false;
  }

  SDL_ShowWindow(TheSDL3WindowVulkan);

  SDL_IOStream *icoStream = SDL_IOFromFile("GeneralsZH.ico", "rb");
  if (icoStream)
  {
    SDL_Surface *icon = IMG_LoadICO_IO(icoStream);
    SDL_CloseIO(icoStream);
    if (icon)
    {
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

  if (TheSDL3Window)
  {
    SDL_ShowWindow(TheSDL3Window);
  }

  return true; // success
}

// Main ====================================================================
/** Application entry point */
//=============================================================================
int main(int argc, char **argv)
{
  int result = EXIT_FAILURE;
  try
  {

    //
    // there is something about checkin in and out the .dsp and .dsw files
    // that blows the working directory information away on each of the
    // developers machines so we're going to hack it for a while and set our
    // working directory to the directory with the .exe since that's not the
    // default in a DevStudio project
    //

    TheUnicodeStringCriticalSection = &critSec2;
    TheDmaCriticalSection = &critSec3;
    TheMemoryPoolCriticalSection = &critSec4;
    TheDebugLogCriticalSection = &critSec5;

#if defined(_DEBUG) && defined(_WIN32)
    // Turn on Memory heap tracking
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= (_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
    tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;
    _CrtSetDbgFlag(tmpFlag);
#endif

    // register windows class and create application window
    if (initializeAppWindows(true) == false)
      return 0;

    // start the log
    DEBUG_INIT(DEBUG_FLAGS_DEFAULT);
    initMemoryManager();

    // Set up version info
    TheVersion = NEW Version;

    DEBUG_LOG(("CRC message is %d\n", GameMessage::MSG_LOGIC_CRC));

    XferCRC xferCRC;
		xferCRC.open("lightCRC");

    TheFileSystem = MSGNEW("GameEngineSubsystem") FileSystem;

    // not part of the subsystem list, because it should normally never be reset!
    TheNameKeyGenerator = new NameKeyGenerator;
    TheNameKeyGenerator->init();

    TheSubsystemList = MSGNEW("GameEngineSubsystem") SubsystemInterfaceList;

    initSubsystem(TheLocalFileSystem, "TheLocalFileSystem", (LocalFileSystem*)MSGNEW("StdLocalFileSystem") StdLocalFileSystem(), NULL);
    initSubsystem(TheArchiveFileSystem, "TheArchiveFileSystem", (ArchiveFileSystem*)MSGNEW("StdBIGFileSystem") StdBIGFileSystem(), NULL); // this MUST come after TheLocalFileSystem creation

    //initSubsystem(TheWritableGlobalData, "TheWritableGlobalData", MSGNEW("GameEngineSubsystem") GlobalData(), &xferCRC, "Data\\INI\\Default\\GameData.ini", "Data\\INI\\GameData.ini");
    initSubsystem(TheWritableGlobalData, "TheWritableGlobalData", MSGNEW("GameEngineSubsystem") GlobalData(), &xferCRC);
    TheWritableGlobalData->m_windowed = true;

    initSubsystem(TheDisplayStringManager, "TheDisplayStringManager", (DisplayStringManager*)MSGNEW("GameEngineSubsystem") W3DDisplayStringManager(), &xferCRC);
    // Display string manager requires TheFontLibrary
    initSubsystem(TheFontLibrary, "TheFontLibrary", (FontLibrary*)MSGNEW("GameEngineSubsystem") W3DFontLibrary(), &xferCRC);

    // GameLogic required by W3DDisplay for frame count
    initSubsystem(TheGameLogic,"TheGameLogic", MSGNEW("GameEngineSubsystem") GameLogic(), &xferCRC); ///< @todo for demo, remove

		// doesn't require resets so just create a single instance here.
		TheGameLODManager = MSGNEW("GameEngineSubsystem") GameLODManager;
		TheGameLODManager->init();
    // Display depends on LOD Manager
    initSubsystem(TheDisplay, "TheDisplay", (Display*)MSGNEW("GameEngineSubsystem") W3DDisplay(), &xferCRC);

    // special-case: parse command-line parameters after loading global data
		parseCommandLine(argc, argv);


    TheDisplay->init();

    Image image;
    image.setName("TitleScreen");
    image.setFilename("TitleScreenuserinterface.tga");
    image.setTextureHeight(1024);
    image.setTextureWidth(1024);
    ICoord2D size = {800 - 1, 600 - 1};
    image.setImageSize(&size);

    SDL_PumpEvents();

    bool bRunning = true;
    while (bRunning)
    {
      // Start rendering with a green clear color
      WW3D::Begin_Render(true, true, Vector3(0.0f, 1.0f, 0.0f), 0.5f);
      TheDisplay->drawImage(&image, 0, 0, size.x, size.y);
      // Color format is ARGB
      TheDisplay->drawFillRect(200, 200, 64, 64, 0x7F0000FF);
      //TheDisplay->draw();
      WW3D::End_Render();

      // Sleep
      SDL_Delay(16);

      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
        if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
          bRunning = false;
      }
    }

    TheSubsystemList->shutdownAll();
    delete TheSubsystemList;
    TheSubsystemList = NULL;

    delete TheFileSystem;
    TheFileSystem = NULL;

    delete TheVersion;
    TheVersion = NULL;

#ifdef MEMORYPOOL_DEBUG
    TheMemoryPoolFactory->debugMemoryReport(REPORT_POOLINFO | REPORT_POOL_OVERFLOW | REPORT_SIMPLE_LEAKS, 0, 0);
#endif
#if defined(_DEBUG) || defined(_INTERNAL)
    TheMemoryPoolFactory->memoryPoolUsageReport("AAAMemStats");
#endif

    // close the log
    shutdownMemoryManager();
    DEBUG_SHUTDOWN();

    // BGC - shut down COM
    //	OleUninitialize();
  }
  catch (...)
  {
  }

  TheUnicodeStringCriticalSection = NULL;
  TheDmaCriticalSection = NULL;
  TheMemoryPoolCriticalSection = NULL;

  return result;
}