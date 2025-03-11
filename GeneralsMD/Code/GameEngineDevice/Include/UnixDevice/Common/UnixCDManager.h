#pragma once

#include "Common/CDManager.h"

class UnixCDDrive : public CDDrive
{
public:
  UnixCDDrive();
  virtual ~UnixCDDrive() override;

  virtual void refreshInfo(void) override;
};

class UnixCDManager : public CDManager
{
public:
  UnixCDManager();
  virtual ~UnixCDManager() override;

  // sub system operations
  virtual void init(void) override;
  virtual void update(void) override;
  virtual void reset(void) override;
  virtual void refreshDrives(void) override;

protected:
  virtual CDDriveInterface *createDrive(void) override;
};