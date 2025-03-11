#pragma once

#include "Common/CDManager.h"

class SDL3CDDrive : public CDDrive
{
public:
  SDL3CDDrive();
  virtual ~SDL3CDDrive() override;

  virtual void refreshInfo(void) override;
};

class SDL3CDManager : public CDManager
{
public:
  SDL3CDManager();
  virtual ~SDL3CDManager() override;

  // sub system operations
  virtual void init(void) override;
  virtual void update(void) override;
  virtual void reset(void) override;
  virtual void refreshDrives(void) override;

protected:
  virtual CDDriveInterface *createDrive(void) override;
};