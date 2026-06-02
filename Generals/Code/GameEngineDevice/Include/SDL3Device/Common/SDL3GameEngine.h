#pragma once

#include "Common/GameEngine.h"
#include "GameLogic/GameLogic.h"
#include "GameNetwork/NetworkInterface.h"
#include "SDL3Device/Common/SDL3BIGFileSystem.h"
#include "SDL3Device/Common/SDL3LocalFileSystem.h"
#include "W3DDevice/Common/W3DModuleFactory.h"
#include "W3DDevice/GameLogic/W3DGameLogic.h"
#include "W3DDevice/GameClient/W3DGameClient.h"
#include "W3DDevice/GameClient/W3DWebBrowser.h"
#include "W3DDevice/Common/W3DFunctionLexicon.h"
#include "W3DDevice/Common/W3DRadar.h"
#include "W3DDevice/Common/W3DFunctionLexicon.h"
#include "W3DDevice/Common/W3DThingFactory.h"
#if defined(SAGE_USE_MINIAUDIO)
#include "MiniAudioDevice/MiniAudioManager.h"
#elif defined(SAGE_USE_OPENAL)
#include "OpenALDevice/OpenALAudioManager.h"
#elif defined(SAGE_USE_MILES)
#include "MilesAudioDevice/MilesAudioManager.h"
#endif

#include <functional>

class SDL3GameEngine : public GameEngine
{
public:
  SDL3GameEngine();
  virtual ~SDL3GameEngine() override;

  virtual void init(void) override;
  virtual void init(int argc, char** argv) override;
  virtual void reset(void) override;
  virtual void update(void) override;
  virtual void serviceWindowsOS(void) override;

  void setPostInitCallback(std::function<void()> callback) { m_postInitCallback = callback; }
  // Factories
protected:
  virtual GameLogic *createGameLogic(void) override;
  virtual GameClient *createGameClient(void) override;
  virtual ModuleFactory *createModuleFactory(void) override;
  virtual ThingFactory *createThingFactory(void) override;
  virtual FunctionLexicon *createFunctionLexicon(void) override;
  virtual LocalFileSystem *createLocalFileSystem(void) override;
  virtual ArchiveFileSystem *createArchiveFileSystem(void) override;
  virtual NetworkInterface *createNetwork(void); // <-- Seems to be unused
  virtual Radar *createRadar(void) override;
  virtual WebBrowser *createWebBrowser( void ) override { return NULL; }
  virtual AudioManager *createAudioManager(void) override;
  virtual ParticleSystemManager *createParticleSystemManager(void) override;

  std::function<void()> m_postInitCallback;
};

// INLINE -----------------------------------------------------------------------------------------
inline GameLogic *SDL3GameEngine::createGameLogic( void ) { return NEW W3DGameLogic; }
inline GameClient *SDL3GameEngine::createGameClient( void ) { return NEW W3DGameClient; }
inline ModuleFactory *SDL3GameEngine::createModuleFactory( void ) { return NEW W3DModuleFactory; }
inline ThingFactory *SDL3GameEngine::createThingFactory( void ) { return NEW W3DThingFactory; }
inline FunctionLexicon *SDL3GameEngine::createFunctionLexicon( void ) { return NEW W3DFunctionLexicon; }
inline LocalFileSystem *SDL3GameEngine::createLocalFileSystem( void ) { return NEW SDL3LocalFileSystem; }
inline ArchiveFileSystem *SDL3GameEngine::createArchiveFileSystem( void ) { return NEW SDL3BIGFileSystem; }
inline ParticleSystemManager* SDL3GameEngine::createParticleSystemManager( void ) { return NEW W3DParticleSystemManager; }

inline NetworkInterface *SDL3GameEngine::createNetwork( void ) { return NetworkInterface::createNetwork(); }
inline Radar *SDL3GameEngine::createRadar( void ) { return NEW W3DRadar; }
#if defined(SAGE_USE_MINIAUDIO)
inline AudioManager *SDL3GameEngine::createAudioManager( void ) { return NEW MiniAudioManager; }
#elif defined(SAGE_USE_OPENAL)
inline AudioManager *SDL3GameEngine::createAudioManager( void ) { return NEW OpenALAudioManager; }
#elif defined(SAGE_USE_MILES)
inline AudioManager* SDL3GameEngine::createAudioManager(void) { return NEW MilesAudioManager; }
#else
inline AudioManager* SDL3GameEngine::createAudioManager(void) { return NULL; }
#endif