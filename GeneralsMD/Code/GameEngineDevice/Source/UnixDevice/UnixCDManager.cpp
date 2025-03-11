#include "UnixDevice/Common/UnixCDManager.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <unistd.h>

CDManagerInterface* CreateCDManager( void )
{
  return NEW UnixCDManager;
}


UnixCDDrive::UnixCDDrive()
{
}

UnixCDDrive::~UnixCDDrive()
{
}

void UnixCDDrive::refreshInfo(void)
{
  // For the given drive path, determine the disk name and ID
  m_disk = CD::NO_DISK;
  int fd = open(m_drivePath.str(), O_RDONLY | O_NONBLOCK);
  if (fd != -1)
  {
    struct cdrom_tochdr tocHeader;
    if (ioctl(fd, CDROMREADTOCHDR, &tocHeader) == 0)
    {
      m_disk = CD::UNKNOWN_DISK;
    }
    close(fd);
  }
}


UnixCDManager::UnixCDManager()
{
}
UnixCDManager::~UnixCDManager()
{
}

void UnixCDManager::init(void)
{
  CDManager::init();
  destroyAllDrives();

  // Detect CD Drives
  for (Char driveLetter = '0'; driveLetter <= '0'; driveLetter++)
  {
    AsciiString drivePath;
    drivePath.format("/dev/sr", driveLetter);

    if (access(drivePath.str(), F_OK) == 0)
    {
      newDrive(drivePath.str());
    }
  }

  refreshDrives();
}

void UnixCDManager::update(void)
{
  CDManager::update();
}

void UnixCDManager::reset(void)
{
  CDManager::reset();
}

void UnixCDManager::refreshDrives(void)
{
  CDManager::refreshDrives();
}

CDDriveInterface* UnixCDManager::createDrive(void)
{
  return NEW UnixCDDrive;
}
