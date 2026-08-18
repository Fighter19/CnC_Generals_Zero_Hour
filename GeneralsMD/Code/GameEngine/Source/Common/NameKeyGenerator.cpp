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

// FILE: NameKeyGenerator.cpp /////////////////////////////////////////////////////////////////////
// Created:   Michael Booth, May 2001
//						Colin Day, May 2001
// Desc:      Name key system to translate between names and unique key ids
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"	// This must go first in EVERY cpp file int the GameEngine

// Public Data ////////////////////////////////////////////////////////////////////////////////////
NameKeyGenerator *TheNameKeyGenerator = NULL;  ///< name key gen. singleton

//------------------------------------------------------------------------------------------------- 
NameKeyGenerator::NameKeyGenerator()
{

	m_nextID = (UnsignedInt)NAMEKEY_INVALID;  // uninitialized system

	// reserve 1 for the NAMEKEY_INVALID entry, which is always empty
	m_vecIdxToName.resize(1);

}  // end NameKeyGenerator

//------------------------------------------------------------------------------------------------- 
NameKeyGenerator::~NameKeyGenerator()
{
	
	// free all system data
	freeSockets();

}  // end ~NameKeyGenerator

//------------------------------------------------------------------------------------------------- 
void NameKeyGenerator::init()
{
	DEBUG_ASSERTCRASH(m_nextID == (UnsignedInt)NAMEKEY_INVALID, ("NameKeyGen already inited"));

	// start keys at the beginning again
	freeSockets();
	m_nextID = 1;

}  // end init

//------------------------------------------------------------------------------------------------- 
void NameKeyGenerator::reset()
{
	freeSockets();
	m_nextID = 1;

}  // end reset

//------------------------------------------------------------------------------------------------- 
void NameKeyGenerator::freeSockets()
{
	m_nameToKeyMap.clear();
	m_vecIdxToName.clear();
	m_vecIdxToName.resize(1);
}  // end freeSockets

//------------------------------------------------------------------------------------------------- 
AsciiString NameKeyGenerator::keyToName(NameKeyType key)
{
	if (key > NAMEKEY_INVALID && key < m_vecIdxToName.size())
	{
		return m_vecIdxToName[key];
	}

	return AsciiString::TheEmptyString;
}

//------------------------------------------------------------------------------------------------- 
NameKeyType NameKeyGenerator::nameToKey(const char* nameString)
{
	// hmm, do we have it already?
	NameToKeyMap::const_iterator it = m_nameToKeyMap.find(nameString);
	if (it != m_nameToKeyMap.end())
	{
		return it->second;
	}

	// nope, guess not. let's allocate it.
	NameKeyType result = (NameKeyType)m_nextID++;
	m_nameToKeyMap[nameString] = result;
	if (result >= m_vecIdxToName.size())
	{
		m_vecIdxToName.resize(result + 1);
	}
	m_vecIdxToName[result] = nameString;

	return result;

}  // end nameToKey

//------------------------------------------------------------------------------------------------- 
NameKeyType NameKeyGenerator::nameToLowercaseKey(const char* nameString)
{
	// Make lowercase copy of the string
	std::string lowerNameString(nameString);
	std::transform(lowerNameString.begin(), lowerNameString.end(), lowerNameString.begin(), [](unsigned char c) { return std::tolower(c); });

	// hmm, do we have it already?
	NameToKeyMap::const_iterator it = m_nameToKeyMap.find(lowerNameString.c_str());
	if (it != m_nameToKeyMap.end())
	{
		return it->second;
	}

	NameKeyType result = (NameKeyType)m_nextID++;
	m_nameToKeyMap[lowerNameString.c_str()] = result;
	if (result >= m_vecIdxToName.size())
	{
		m_vecIdxToName.resize(result + 1);
	}
	m_vecIdxToName[result] = lowerNameString.c_str();

	return result;

}  // end nameToLowercaseKey

//------------------------------------------------------------------------------------------------- 
// Get a string out of the INI. Store it into a NameKeyType
//------------------------------------------------------------------------------------------------- 
void NameKeyGenerator::parseStringAsNameKeyType( INI *ini, void *instance, void *store, const void* userData )
{
  *(NameKeyType *)store = TheNameKeyGenerator->nameToKey( ini->getNextToken() );
}


//------------------------------------------------------------------------------------------------- 
NameKeyType StaticNameKey::key() const
{
	if (m_key == NAMEKEY_INVALID)
	{
		DEBUG_ASSERTCRASH(TheNameKeyGenerator, ("no TheNameKeyGenerator yet"));
		if (TheNameKeyGenerator)
			m_key = TheNameKeyGenerator->nameToKey(m_name);
	}
	return m_key;
}
