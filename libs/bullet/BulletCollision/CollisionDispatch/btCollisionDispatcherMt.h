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

#ifndef BT_COLLISION_DISPATCHER_MT_H
#define BT_COLLISION_DISPATCHER_MT_H

#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "LinearMath/btThreads.h"
#include <vector>
#include <mutex>
#include <cassert>
#include <android/log.h>

class btCollisionDispatcherMt : public btCollisionDispatcher
{
public:
	btCollisionDispatcherMt(btCollisionConfiguration* config, int grainSize = 40);

	virtual btPersistentManifold* getNewManifold(const btCollisionObject* body0, const btCollisionObject* body1) BT_OVERRIDE;
	virtual void releaseManifold(btPersistentManifold* manifold) BT_OVERRIDE;

	virtual void dispatchAllCollisionPairs(btOverlappingPairCache* pairCache, const btDispatcherInfo& info, btDispatcher* dispatcher) BT_OVERRIDE;

	int getNumManifolds() const
	{
        int i = 0;
        {
            std::scoped_lock<std::mutex> scopedLock(m_mutex);

            i = m_manifoldsPtr.size();
        }
        return i;
	}

	int getNumManifolds()
	{
		int i = 0;
		{
			std::scoped_lock<std::mutex> scopedLock(m_mutex);

			i = m_manifoldsPtr.size();
		}
		return i;
	}

	btPersistentManifold** getInternalManifoldPointer()
	{
		btPersistentManifold** ptr = nullptr;
		{
			std::scoped_lock<std::mutex> scopedLock(m_mutex);

			ptr = m_manifoldsPtr.empty() ? nullptr : &m_manifoldsPtr[0];
		}

		if(!ptr)
		{
			__android_log_print(ANDROID_LOG_DEBUG, "TAG", "return nullptr");
		}

		return ptr;
	}

	btPersistentManifold** getInternalManifoldPointer() const
	{
		__android_log_print(ANDROID_LOG_DEBUG, "TAG", "assert getInternalManifoldPointer");
		assert(false);

		return nullptr;
	}

	btPersistentManifold* getManifoldByIndexInternal(int index)
	{
		btPersistentManifold* ptr = nullptr;
		{
			std::scoped_lock<std::mutex> scopedLock(m_mutex);

			if(index >= 0 && index < m_manifoldsPtr.size())
			{
				ptr = m_manifoldsPtr[index];
			}
		}

		if(!ptr)
		{
			__android_log_print(ANDROID_LOG_DEBUG, "TAG", "return nullptr");
		}

		return ptr;
	}

	btPersistentManifold* getManifoldByIndexInternal(int index) const
	{
		__android_log_print(ANDROID_LOG_DEBUG, "TAG", "assert getManifoldByIndexInternal const");
		assert(false);
		return nullptr;
	}

private:
	int m_grainSize;
	std::vector<btPersistentManifold*> m_manifoldsPtr;
	mutable std::mutex m_mutex;
};

#endif  //BT_COLLISION_DISPATCHER_MT_H
