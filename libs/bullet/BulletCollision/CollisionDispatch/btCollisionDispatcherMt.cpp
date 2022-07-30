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
	m_manifoldsPtr.reserve(500);
}

btPersistentManifold* btCollisionDispatcherMt::getNewManifold(const btCollisionObject* body0, const btCollisionObject* body1)
{
	//optional relative contact breaking threshold, turned on by default (use setDispatcherFlags to switch off feature for improved performance)

	//btScalar contactBreakingThreshold = (m_dispatcherFlags & btCollisionDispatcher::CD_USE_RELATIVE_CONTACT_BREAKING_THRESHOLD) ? btMin(body0->getCollisionShape()->getContactBreakingThreshold(gContactBreakingThreshold), body1->getCollisionShape()->getContactBreakingThreshold(gContactBreakingThreshold))
	//																															: gContactBreakingThreshold;

    btScalar contactProcessingThreshold = btMin(body0->getContactProcessingThreshold(), body1->getContactProcessingThreshold());

    void* mem = m_persistentManifoldPoolAllocator->allocate(sizeof(btPersistentManifold));
    if (NULL == mem)
    {
        //we got a pool memory overflow, by default we fallback to dynamically allocate memory. If we require a contiguous contact pool then assert.
        if ((m_dispatcherFlags & CD_DISABLE_CONTACTPOOL_DYNAMIC_ALLOCATION) == 0)
        {
            mem = btAlignedAlloc(sizeof(btPersistentManifold), 16);
        }
        else
        {
            assert(0);
            //make sure to increase the m_defaultMaxPersistentManifoldPoolSize in the btDefaultCollisionConstructionInfo/btDefaultCollisionConfiguration
            return 0;
        }
    }

    btPersistentManifold* manifold = new (mem) btPersistentManifold(body0, body1, 0, gContactBreakingThreshold, contactProcessingThreshold);

	{
		std::scoped_lock<std::mutex> scopedLock(m_mutex);

		manifold->m_index1a = m_manifoldsPtr.size();
		m_manifoldsPtr.push_back(manifold);
	}

	return manifold;
}

void btCollisionDispatcherMt::releaseManifold(btPersistentManifold* manifold)
{
	clearManifold(manifold);

	{
		std::scoped_lock<std::mutex> scopedLock(m_mutex);

		int findIndex = manifold->m_index1a;
		assert(findIndex < m_manifoldsPtr.size());
		assert(findIndex >= 0);

		m_manifoldsPtr[findIndex] = m_manifoldsPtr[m_manifoldsPtr.size() - 1];
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

void btCollisionDispatcherMt::dispatchAllCollisionPairs(btOverlappingPairCache* pairCache, const btDispatcherInfo& info, btDispatcher* dispatcher)
{
	const int pairCount = pairCache->getNumOverlappingPairs();
	btBroadphasePair* pairArray = pairCache->getOverlappingPairArrayPtr();

	for (int i = 0; i < pairCount; ++i)
	{
		defaultNearCallback(pairArray[i], *this, info);
	}
}
