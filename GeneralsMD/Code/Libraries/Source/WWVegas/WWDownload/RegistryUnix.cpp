#include "Registry.h"

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

// Registry.cpp
// Simple interface for storing/retreiving registry values
// Author: Matthew D. Campbell, December 2001

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "Registry.h"

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

// Registry.cpp
// Simple interface for storing/retreiving registry values
// Author: Matthew D. Campbell, December 2001

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "Registry.h"

#include <filesystem>
#include <fstream>

static std::filesystem::path getRootPath()
{
  std::filesystem::path userDataDir;
	const char* xdgDataHome = getenv("XDG_DATA_HOME");
	if (xdgDataHome)
		userDataDir = std::filesystem::path(xdgDataHome);
	else
		userDataDir = std::filesystem::path(getenv("HOME")) / ".local" / "share" / "generals_zh";

  return userDataDir;
}

bool GetStringFromRegistry(std::string path, std::string key, std::string& val)
{
  std::filesystem::path fullPath = getRootPath();

	fullPath.append(path);
  fullPath.append(key);

  std::ifstream file(fullPath);
  if (!file.is_open())
    return false;

  file >> val;
  return true;
}

bool GetUnsignedIntFromRegistry(std::string path, std::string key, unsigned int& val)
{
  std::filesystem::path fullPath = getRootPath();

  fullPath.append(path);
  fullPath.append(key);

  std::ifstream file(fullPath);
  if (!file.is_open())
    return false;

  file >> val;
  return true;
}

bool SetStringInRegistry( std::string path, std::string key, std::string val)
{
  std::filesystem::path fullPath = getRootPath();

  fullPath.append(path);
  fullPath.append(key);

  std::ofstream file(fullPath);
  if (!file.is_open())
    return false;

  file << val;
  return true;
}

bool SetUnsignedIntInRegistry( std::string path, std::string key, unsigned int val)
{
  std::filesystem::path fullPath = getRootPath();

  fullPath.append(path);
  fullPath.append(key);

  std::ofstream file(fullPath);
  if (!file.is_open())
    return false;

  file << val;
  return true;
}

