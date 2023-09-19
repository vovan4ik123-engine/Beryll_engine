/*
Copyright (c) 2003-2014 Erwin Coumans  http://bullet.googlecode.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "btThreads.h"
#include "btQuickprof.h"
#include <algorithm>

#if BT_THREADSAFE

#include <atomic>
#include <thread>

bool btSpinMutex::tryLock()
{
	std::atomic<int>* aDest = reinterpret_cast<std::atomic<int>*>(&mLock);
	int expected = 0;
	return std::atomic_compare_exchange_weak_explicit(aDest, &expected, int(1), std::memory_order_acq_rel, std::memory_order_acquire);
}

void btSpinMutex::lock()
{
	// note: this lock does not sleep the thread.
	while (!tryLock())
	{
		// spin
	}
}

void btSpinMutex::unlock()
{
	std::atomic<int>* aDest = reinterpret_cast<std::atomic<int>*>(&mLock);
	std::atomic_store_explicit(aDest, int(0), std::memory_order_release);
}

#endif // #else BT_THREADSAFE


static btITaskScheduler* gBtTaskScheduler = nullptr;

btITaskScheduler::btITaskScheduler(const char* name)
{
	m_name = name;
	m_savedThreadCounter = 0;
	m_isActive = false;
}

void btSetTaskScheduler(btITaskScheduler* ts)
{
	// Dont reassign gBtTaskScheduler.
	assert(gBtTaskScheduler == nullptr);

	gBtTaskScheduler = ts;
}

btITaskScheduler* btGetTaskScheduler()
{
	return gBtTaskScheduler;
}

void btParallelFor(int iBegin, int iEnd, int grainSize, const btIParallelForBody& body)
{
#if BT_THREADSAFE

	btAssert(gBtTaskScheduler != nullptr);  // call btSetTaskScheduler() with a valid task scheduler first!
	gBtTaskScheduler->parallelFor(iBegin, iEnd, grainSize, body);

#else // #if BT_THREADSAFE

	assert(false);

#endif // #if BT_THREADSAFE
}

btScalar btParallelSum(int iBegin, int iEnd, int grainSize, const btIParallelSumBody& body)
{
#if BT_THREADSAFE

	btAssert(gBtTaskScheduler != nullptr);  // call btSetTaskScheduler() with a valid task scheduler first!
	return gBtTaskScheduler->parallelSum(iBegin, iEnd, grainSize, body);

#else // #if BT_THREADSAFE

	assert(false);

#endif //#else // #if BT_THREADSAFE
}
