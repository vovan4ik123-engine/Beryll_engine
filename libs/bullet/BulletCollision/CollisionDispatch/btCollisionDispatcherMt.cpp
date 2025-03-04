/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  https://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "btCollisionDispatcherMt.h"
#include "LinearMath/btQuickprof.h"

#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"

#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "LinearMath/btPoolAllocator.h"
#include "BulletCollision/CollisionDispatch/btCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h"

btCollisionDispatcherMt::btCollisionDispatcherMt(btCollisionConfiguration* config, int grainSize)
	: btCollisionDispatcher(config)
{
	m_grainSize = grainSize;
}

btPersistentManifold* btCollisionDispatcherMt::getNewManifold(const btCollisionObject* body0, const btCollisionObject* body1)
{
	btScalar contactProcessingThreshold = btMin(body0->getContactProcessingThreshold(), body1->getContactProcessingThreshold());

	void* mem = m_persistentManifoldPoolAllocator->allocate(sizeof(btPersistentManifold));
	if (nullptr == mem)
	{
		mem = btAlignedAlloc(sizeof(btPersistentManifold), 16);
	}

	btPersistentManifold* manifold = new (mem) btPersistentManifold(body0, body1, 0, gContactBreakingThreshold, contactProcessingThreshold);

    // Scope for lock.
	{
        ScopedSpinlock lock{m_spinLock};

		manifold->m_index1a = m_manifoldsPtr.size();
		m_manifoldsPtr.push_back(manifold);
	}

	return manifold;
}

void btCollisionDispatcherMt::releaseManifold(btPersistentManifold* manifold)
{
    clearManifold(manifold);
    int findIndex = manifold->m_index1a;

    // Scope for lock.
	{
        ScopedSpinlock lock{m_spinLock};

		btAssert(findIndex < m_manifoldsPtr.size());
		m_manifoldsPtr.swap(findIndex, m_manifoldsPtr.size() - 1);
		m_manifoldsPtr[findIndex]->m_index1a = findIndex;
		m_manifoldsPtr.pop_back();
	}

	manifold->~btPersistentManifold();
	if (m_persistentManifoldPoolAllocator->validPtr(manifold))
	{
		m_persistentManifoldPoolAllocator->freeMemory(manifold);
	}
	else
	{
		btAlignedFree(manifold);
	}
}

struct CollisionDispatcherUpdater : public btIParallelForBody
{
	btBroadphasePair* mPairArray;
	btNearCallback mCallback;
	btCollisionDispatcher* mDispatcher;
	const btDispatcherInfo* mInfo;

	CollisionDispatcherUpdater()
	{
		mPairArray = NULL;
		mCallback = NULL;
		mDispatcher = NULL;
		mInfo = NULL;
	}
	void forLoop(int iBegin, int iEnd) const
	{
		for (int i = iBegin; i < iEnd; ++i)
		{
			btBroadphasePair* pair = &mPairArray[i];
			mCallback(*pair, *mDispatcher, *mInfo);
		}
	}
};

void btCollisionDispatcherMt::dispatchAllCollisionPairs(btOverlappingPairCache* pairCache, const btDispatcherInfo& info, btDispatcher* dispatcher)
{
	const int pairCount = pairCache->getNumOverlappingPairs();
	if (pairCount == 0)
	{
		return;
	}
	CollisionDispatcherUpdater updater;
	updater.mCallback = getNearCallback();
	updater.mPairArray = pairCache->getOverlappingPairArrayPtr();
	updater.mDispatcher = this;
	updater.mInfo = &info;

	btParallelFor(0, pairCount, m_grainSize, updater);

	// update the indices (used when releasing manifolds)
	for (int i = 0; i < m_manifoldsPtr.size(); ++i)
	{
		m_manifoldsPtr[i]->m_index1a = i;
	}
}
