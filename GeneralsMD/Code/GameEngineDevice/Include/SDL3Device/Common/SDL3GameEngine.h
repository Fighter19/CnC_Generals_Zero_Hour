#pragma once

#include "Common/GameEngine.h"
#include "GameLogic/GameLogic.h"
#include "GameNetwork/NetworkInterface.h"
#include "W3DDevice/Common/W3DModuleFactory.h"
#include "W3DDevice/GameLogic/W3DGameLogic.h"
#include "W3DDevice/GameClient/W3DGameClient.h"
#include "W3DDevice/GameClient/W3DWebBrowser.h"
#include "W3DDevice/Common/W3DFunctionLexicon.h"
#include "W3DDevice/Common/W3DRadar.h"
#include "W3DDevice/Common/W3DFunctionLexicon.h"
#include "W3DDevice/Common/W3DThingFactory.h"

class SDL3GameEngine : public GameEngine
{
public:
  SDL3GameEngine();
  virtual ~SDL3GameEngine() override;

  virtual void init(void) override;
  virtual void reset(void) override;
  virtual void update(void) override;
  virtual void serviceWindowsOS(void) override;

  // Factories
protected:
  virtual GameLogic *createGameLogic(void) override;
  virtual GameClient *createGameClient(void) override;
  virtual ModuleFactory *createModuleFactory(void) override;
  virtual ThingFactory *createThingFactory(void) override;
  virtual FunctionLexicon *createFunctionLexicon(void) override;
  virtual LocalFileSystem *createLocalFileSystem(void) override;
  virtual ArchiveFileSystem *createArchiveFileSystem(void) override;
  // virtual NetworkInterface *createNetwork(void) override; // <-- Seems to be unused
  virtual Radar *createRadar(void) override;
  virtual AudioManager *createAudioManager(void) override;
  virtual ParticleSystemManager *createParticleSystemManager(void) override;
};