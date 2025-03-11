#pragma once

#include "Common/GameEngine.h"

class UnixGameEngine : public GameEngine
{
public:
  UnixGameEngine();
  virtual ~UnixGameEngine() override;

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