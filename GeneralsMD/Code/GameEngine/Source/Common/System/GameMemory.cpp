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

#include "PreRTS.h"

#include "Common/GameMemory.h"

MemoryPoolFactory *TheMemoryPoolFactory = NULL;
DynamicMemoryAllocator *TheDynamicMemoryAllocator = NULL;

void initMemoryManager()
{
	// create the memory pool factory
	TheMemoryPoolFactory = new MemoryPoolFactory;

	// create the dynamic memory allocator
	TheDynamicMemoryAllocator = new DynamicMemoryAllocator;
}

void shutdownMemoryManager()
{
	delete TheDynamicMemoryAllocator;
	delete TheMemoryPoolFactory;
}

MemoryPool::MemoryPool()
{
}

MemoryPool::~MemoryPool()
{
}

void MemoryPool::freeBlock(void *pMem)
{
	// free the memory
	free(pMem);
}

void *MemoryPool::allocateBlock(const char *name)
{
	// allocate the memory
	void *pMem = malloc(m_allocationSize);
	memset(pMem, 0, m_allocationSize);
	return pMem;
}

void MemoryPool::init(MemoryPoolFactory *pFactory, const char *poolName, Int allocationSize, Int initialAllocationCount, Int overflowAllocationCount)
{
	m_allocationSize = allocationSize;
	m_initialAllocationCount = initialAllocationCount;
	m_overflowAllocationCount = overflowAllocationCount;
}

MemoryPoolFactory::MemoryPoolFactory()
{
}

MemoryPoolFactory::~MemoryPoolFactory()
{
}

MemoryPool *MemoryPoolFactory::createMemoryPool(const char *poolName, Int allocationSize, Int initialAllocationCount, Int overflowAllocationCount)
{
	// create the memory pool
	MemoryPool *pPool = new MemoryPool;
	pPool->init(this, poolName, allocationSize, initialAllocationCount, overflowAllocationCount);
	return pPool;
}



DynamicMemoryAllocator::DynamicMemoryAllocator()
{
}

DynamicMemoryAllocator::~DynamicMemoryAllocator()
{
}

void *DynamicMemoryAllocator::allocateBytesImplementation(Int numBytes DECLARE_LITERALSTRING_ARG2)
{
	void *pMem = malloc(numBytes);
	memset(pMem, 0, numBytes);
	return pMem;
}

void DynamicMemoryAllocator::freeBytes(void *pMem)
{
	free(pMem);
}

int DynamicMemoryAllocator::getActualAllocationSize(int nSize)
{
	return nSize;
}

void *DynamicMemoryAllocator::allocateBytesDoNotZeroImplementation(Int numBytes DECLARE_LITERALSTRING_ARG2)
{
	void *pMem = malloc(numBytes);
	return pMem;
}

void * createW3DMemPool(const char *name, Int size)
{
	return TheMemoryPoolFactory->createMemoryPool( name, size, 0, 0);
}

void * allocateFromW3DMemPool(void *pool, Int size)
{
	return ((MemoryPool*)pool)->allocateBlock("allocateFromW3DMemPool");
}

void * allocateFromW3DMemPool(void *pool, Int size, const char *msg, int unused)
{
#ifdef MEMORYPOOL_DEBUG
	return ((MemoryPool*)pool)->allocateBlock(msg);
#else
	return ((MemoryPool*)pool)->allocateBlock("allocateFromW3DMemPool");
#endif
}

void freeFromW3DMemPool(void *pool, void *pMem)
{
	((MemoryPool*)pool)->freeBlock( pMem);
}

void* __cdecl operator new(size_t nSize, const char *fname, int)
{
#ifdef MEMORYPOOL_DEBUG
	return TheDynamicMemoryAllocator->allocateBytesImplementation(nSize, fname);
#else
	return TheDynamicMemoryAllocator->allocateBytesImplementation(nSize);
#endif
}

void* __cdecl operator new[](size_t nSize, const char *fname, int)
{
#ifdef MEMORYPOOL_DEBUG	
	return TheDynamicMemoryAllocator->allocateBytesImplementation(nSize, fname);
#else
	return TheDynamicMemoryAllocator->allocateBytesImplementation(nSize);
#endif
}

void __cdecl operator delete(void *p, const char *, int) noexcept
{
	TheDynamicMemoryAllocator->freeBytes(p);
}

void __cdecl operator delete[](void *p, const char *, int) noexcept
{
	TheDynamicMemoryAllocator->freeBytes(p);
}

void MemoryPoolFactory::debugMemoryReport(int, int, int, FILE*)
{

}

void MemoryPoolFactory::memoryPoolUsageReport(const char* filename, FILE *appendToFileInstead)
{
	if (appendToFileInstead)
	{
		fprintf(appendToFileInstead, "Memory Pool Usage Report:\n");
	}
	else
	{
		FILE *file = fopen(filename, "w");
		if (file)
		{
			fprintf(file, "Memory Pool Usage Report:\n");
			fclose(file);
		}
	}
}

void MemoryPoolFactory::debugSetInitFillerIndex(Int index)
{
	// This function is a placeholder for future use.
	// It can be used to set an index for initialization filler, if needed.
	DEBUG_LOG(("debugSetInitFillerIndex called with index: %d", index));
}

void DynamicMemoryAllocator::debugIgnoreLeaksForThisBlock(void* pBlockPtr)
{
	// This function is a placeholder for future use.
	// It can be used to ignore leaks for a specific block, if needed.
	DEBUG_LOG(("debugIgnoreLeaksForThisBlock called for block: %p", pBlockPtr));
}