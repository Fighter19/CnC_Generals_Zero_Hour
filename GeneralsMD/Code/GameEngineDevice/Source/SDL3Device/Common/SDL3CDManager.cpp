#include "SDL3Device/Common/SDL3CDManager.h"
#include <fcntl.h>

CDManagerInterface* CreateCDManager( void )
{
  return NEW SDL3CDManager;
}


SDL3CDDrive::SDL3CDDrive()
{
}

SDL3CDDrive::~SDL3CDDrive()
{
}

void SDL3CDDrive::refreshInfo(void)
{
  // For the given drive path, determine the disk name and ID
  m_disk = CD::NO_DISK;
  // int fd = open(m_drivePath.str(), O_RDONLY | O_NONBLOCK);
  // if (fd != -1)
  // {
  //   struct cdrom_tochdr tocHeader;
  //   if (ioctl(fd, CDROMREADTOCHDR, &tocHeader) == 0)
  //   {
  //     m_disk = CD::UNKNOWN_DISK;
  //   }
  //   close(fd);
  // }
}


SDL3CDManager::SDL3CDManager()
{
}
SDL3CDManager::~SDL3CDManager()
{
}

void SDL3CDManager::init(void)
{
  CDManager::init();
  destroyAllDrives();

  // Detect CD Drives
  for (Char driveLetter = '0'; driveLetter <= '0'; driveLetter++)
  {
    AsciiString drivePath;
    drivePath.format("/dev/sr", driveLetter);

    // if (access(drivePath.str(), F_OK) == 0)
    // {
    //   newDrive(drivePath.str());
    // }
  }

  refreshDrives();
}

void SDL3CDManager::update(void)
{
  CDManager::update();
}

void SDL3CDManager::reset(void)
{
  CDManager::reset();
}

void SDL3CDManager::refreshDrives(void)
{
  CDManager::refreshDrives();
}

CDDriveInterface* SDL3CDManager::createDrive(void)
{
  return NEW SDL3CDDrive;
}
