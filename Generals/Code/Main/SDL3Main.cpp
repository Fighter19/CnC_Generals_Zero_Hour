/*
**	Command & Conquer Generals(tm)
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

// FILE: WinMain.cpp //////////////////////////////////////////////////////////
// 
// Entry point for game application
//
// Author: Colin Day, April 2001
//
///////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES ////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#include <eh.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif

#include <SDL3/SDL.h>

// USER INCLUDES //////////////////////////////////////////////////////////////
#include "WinMain.h"
#include "Lib/BaseType.h"
#include "Common/CopyProtection.h"
#include "Common/CriticalSection.h"
#include "Common/GlobalData.h"
#include "Common/GameEngine.h"
#include "Common/GameSounds.h"
#include "Common/Debug.h"
#include "Common/GameMemory.h"
#include "Common/StackDump.h"
#include "Common/MessageStream.h"
#include "Common/Team.h"
#include "GameClient/InGameUI.h"
#include "GameClient/GameClient.h"
#include "GameLogic/GameLogic.h"  ///< @todo for demo, remove
#include "GameClient/Mouse.h"
#include "GameClient/IMEManager.h"
#include "SDL3Device/Common/SDL3GameEngine.h"
#include "Common/Version.h"
#include "buildVersion.h"
#include "generatedVersion.h"

#ifdef _INTERNAL
// for occasional debugging...
//#pragma optimize("", off)
//#pragma message("************************************** WARNING, optimization disabled for debugging purposes")
#endif

// GLOBALS ////////////////////////////////////////////////////////////////////
HINSTANCE ApplicationHInstance = NULL;  ///< our application instance
HWND ApplicationHWnd = NULL;  ///< our application window handle
Bool ApplicationIsWindowed = false;
SDL3Mouse *TheWin32Mouse = NULL;  ///< shared mouse instance for the SDL3 runtime
DWORD TheMessageTime = 0;	///< For getting the time that a message was posted from Windows.
SDL_Window *ApplicationWindow = NULL;

const Char *g_strFile = "data\\Generals.str";
const Char *g_csfFile = "data\\%s\\Generals.csf";
const char *gAppPrefix = ""; /// So WB can have a different debug log file name.

#define GENERALS_GUID "685EAFF2-3216-4265-B047-251C5F4B82F3"
#define DEFAULT_XRESOLUTION 800
#define DEFAULT_YRESOLUTION 600

#ifdef _WIN32
static HANDLE GeneralsMutex = NULL;
#else
static int GeneralsLockFd = -1;
#endif

static Bool isWinMainActive = true;
static SDL_Renderer *gLoadScreenRenderer = NULL;
static SDL_Texture *gLoadScreenTexture = NULL;

static void shutdownLoadingScreen(void)
{
	if (gLoadScreenTexture != NULL)
	{
		SDL_DestroyTexture(gLoadScreenTexture);
		gLoadScreenTexture = NULL;
	}

	if (gLoadScreenRenderer != NULL)
	{
		SDL_DestroyRenderer(gLoadScreenRenderer);
		gLoadScreenRenderer = NULL;
	}
}

static void destroyApplicationWindow(void)
{
	shutdownLoadingScreen();

	if (ApplicationWindow != NULL)
	{
		SDL_DestroyWindow(ApplicationWindow);
		ApplicationWindow = NULL;
	}

	ApplicationHWnd = NULL;
}

static void setWorkingDirectory(const char *path)
{
#ifdef _WIN32
	SetCurrentDirectoryA(path);
#else
	chdir(path);
#endif
}

static void setWorkingDirectoryToExecutable(void)
{
	const char *basePath = SDL_GetBasePath();
	if (basePath == NULL)
	{
		return;
	}
	setWorkingDirectory(basePath);
}

static void renderLoadingScreen(void)
{
	SDL_Surface *surface;

	if (ApplicationWindow == NULL)
	{
		return;
	}

	gLoadScreenRenderer = SDL_CreateRenderer(ApplicationWindow, NULL);
	if (gLoadScreenRenderer == NULL)
	{
		return;
	}

	surface = SDL_LoadBMP("Install_Final.bmp");
	if (surface == NULL)
	{
		DEBUG_LOG(("Failed to load loading screen bitmap: %s\n", SDL_GetError()));
		shutdownLoadingScreen();
		return;
	}

	gLoadScreenTexture = SDL_CreateTextureFromSurface(gLoadScreenRenderer, surface);
	SDL_DestroySurface(surface);
	if (gLoadScreenTexture == NULL)
	{
		DEBUG_LOG(("Failed to create loading screen texture: %s\n", SDL_GetError()));
		shutdownLoadingScreen();
		return;
	}

	SDL_RenderClear(gLoadScreenRenderer);
	SDL_RenderTexture(gLoadScreenRenderer, gLoadScreenTexture, NULL, NULL);
	SDL_RenderPresent(gLoadScreenRenderer);
}

static Bool initializeAppWindow(Bool runWindowed)
{
	SDL_WindowFlags windowFlags = SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

	if (!runWindowed)
	{
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}

	ApplicationWindow = SDL_CreateWindow("Command and Conquer Generals",
		DEFAULT_XRESOLUTION,
		DEFAULT_YRESOLUTION,
		windowFlags);
	if (ApplicationWindow == NULL)
	{
		DEBUG_LOG(("Failed to create SDL window: %s\n", SDL_GetError()));
		return false;
	}

	ApplicationHWnd = reinterpret_cast<HWND>(ApplicationWindow);
	//renderLoadingScreen();
	return true;
}

static Bool acquireSingleInstanceLock(void)
{
#ifdef _WIN32
	GeneralsMutex = CreateMutexA(NULL, FALSE, GENERALS_GUID);
	if (GeneralsMutex == NULL)
	{
		return true;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(GeneralsMutex);
		GeneralsMutex = NULL;
		return false;
	}

	return true;
#else
	GeneralsLockFd = open("/tmp/cnc-generals-zero-hour.lock", O_CREAT | O_RDWR, 0666);
	if (GeneralsLockFd < 0)
	{
		return true;
	}

	if (flock(GeneralsLockFd, LOCK_EX | LOCK_NB) != 0)
	{
		close(GeneralsLockFd);
		GeneralsLockFd = -1;
		return false;
	}

	return true;
#endif
}

static void releaseSingleInstanceLock(void)
{
#ifdef _WIN32
	if (GeneralsMutex != NULL)
	{
		CloseHandle(GeneralsMutex);
		GeneralsMutex = NULL;
	}
#else
	if (GeneralsLockFd >= 0)
	{
		flock(GeneralsLockFd, LOCK_UN);
		close(GeneralsLockFd);
		GeneralsLockFd = -1;
	}
#endif
}

// Necessary to allow memory managers and such to have useful critical sections
static CriticalSection critSec1, critSec2, critSec3, critSec4, critSec5;

int main(int argc, char **argv)
{
	Bool lockHeld = false;

	try {

	#ifdef _WIN32
		ApplicationHInstance = GetModuleHandleA(NULL);
	#endif

	#ifdef _MSC_VER
		_set_se_translator( DumpExceptionInfo ); // Hook that allows stack trace.
	#endif

		// Set DXVK_WSI_DRIVER env variable to SDL3
		setenv("DXVK_WSI_DRIVER", "SDL3", 1);

		TheAsciiStringCriticalSection = &critSec1;
		TheUnicodeStringCriticalSection = &critSec2;
		TheDmaCriticalSection = &critSec3;
		TheMemoryPoolCriticalSection = &critSec4;
		TheDebugLogCriticalSection = &critSec5;

		setWorkingDirectoryToExecutable();

		for (int index = 1; index < argc; ++index)
		{
			if (SDL_strcasecmp(argv[index], "-win") == 0)
			{
				ApplicationIsWindowed = true;
			}
			if (SDL_strcasecmp(argv[index], "-dir") == 0)
			{
				if (index + 1 < argc)
				{
					setWorkingDirectory(argv[index + 1]);
				}
				index += 1; // skip the next arg since we just used it
			}
		}

		if (argc>2 && strcmp(argv[1],"-DX")==0) {  
			Int i;
			DEBUG_LOG(("\n--- DX STACK DUMP\n"));
			for (i=2; i<argc; i++) {
				Int pc;
				pc = 0;
				sscanf(argv[i], "%x",  &pc);
				char name[_MAX_PATH], file[_MAX_PATH];
				unsigned int line;
				unsigned int addr;
				GetFunctionDetails((void*)pc, name, file, &line, &addr);
				DEBUG_LOG(("0x%x - %s, %s, line %d address 0x%x\n", pc, name, file, line, addr));
			}
			DEBUG_LOG(("\n--- END OF DX STACK DUMP\n"));
			return 0;
		}

		#ifdef _MSC_VER
		#ifdef _DEBUG
			// Turn on Memory heap tracking
			int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
			tmpFlag |= (_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
			tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;
			_CrtSetDbgFlag( tmpFlag );
		#endif
		#endif

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0)
		{
			DEBUG_LOG(("SDL failed to initialize: %s\n", SDL_GetError()));
			return 1;
		}

		if (initializeAppWindow(ApplicationIsWindowed) == false)
		{
			SDL_Quit();
			return 1;
		}

		shutdownLoadingScreen();

		// start the log
		DEBUG_INIT(DEBUG_FLAGS_DEFAULT);
		initMemoryManager();

 
		// Set up version info
		TheVersion = NEW Version;
		TheVersion->setVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILDNUM, VERSION_LOCALBUILDNUM,
			AsciiString(VERSION_BUILDUSER), AsciiString(VERSION_BUILDLOC),
			AsciiString(__TIME__), AsciiString(__DATE__));

#ifdef DO_COPY_PROTECTION
		if (!CopyProtect::isLauncherRunning())
		{
			DEBUG_LOG(("Launcher is not running - about to bail\n"));
			delete TheVersion;
			TheVersion = NULL;
			shutdownMemoryManager();
			DEBUG_SHUTDOWN();
			return 0;
		}
#endif

		if (!acquireSingleInstanceLock())
		{
			DEBUG_LOG(("Generals is already running...Bail!\n"));
			delete TheVersion;
			TheVersion = NULL;
			shutdownMemoryManager();
			DEBUG_SHUTDOWN();
			destroyApplicationWindow();
			SDL_Quit();
			return 0;
		}
		lockHeld = true;
		DEBUG_LOG(("Create GeneralsMutex okay.\n"));

#ifdef DO_COPY_PROTECTION
		if (!CopyProtect::notifyLauncher())
		{
			DEBUG_LOG(("Could not talk to the launcher - about to bail\n"));
			delete TheVersion;
			TheVersion = NULL;
			shutdownMemoryManager();
			DEBUG_SHUTDOWN();
			return 0;
		}
#endif

		DEBUG_LOG(("CRC message is %d\n", GameMessage::MSG_LOGIC_CRC));

		// run the game main loop
		GameMain(argc, argv);

#ifdef DO_COPY_PROTECTION
		// Clean up copy protection
		CopyProtect::shutdown();
#endif

		delete TheVersion;
		TheVersion = NULL;

	#ifdef MEMORYPOOL_DEBUG
		TheMemoryPoolFactory->debugMemoryReport(REPORT_POOLINFO | REPORT_POOL_OVERFLOW | REPORT_SIMPLE_LEAKS, 0, 0);
	#endif
	#if defined(_DEBUG) || defined(_INTERNAL)
		TheMemoryPoolFactory->memoryPoolUsageReport("AAAMemStats");
	#endif

		// close the log
		releaseSingleInstanceLock();
		lockHeld = false;
		shutdownMemoryManager();
		DEBUG_SHUTDOWN();
		destroyApplicationWindow();
		SDL_Quit();
	}	
	catch (...) 
	{ 
		if (lockHeld)
		{
			releaseSingleInstanceLock();
		}
		destroyApplicationWindow();
		SDL_Quit();
	}

	TheAsciiStringCriticalSection = NULL;
	TheUnicodeStringCriticalSection = NULL;
	TheDmaCriticalSection = NULL;
	TheMemoryPoolCriticalSection = NULL;
	TheDebugLogCriticalSection = NULL;

	return 0;

}  // end main

GameEngine *CreateGameEngine( void )
{
	SDL3GameEngine *engine;

	engine = NEW SDL3GameEngine;
	engine->setIsActive(isWinMainActive);

	return engine;

}  // end CreateGameEngine
