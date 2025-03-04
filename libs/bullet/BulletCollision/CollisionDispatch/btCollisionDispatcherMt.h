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

#include <atomic>

class Spinlock
{
public:
    void lock()
    {
        while(m_flag.test_and_set(std::memory_order_acquire))
        {
            // Busy wait.
        }
    }

    void unlock()
    {
        m_flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

class ScopedSpinlock
{
public:
    explicit ScopedSpinlock(Spinlock& spinlock) : m_spinlock(spinlock)
    {
        m_spinlock.lock();
    }

    ~ScopedSpinlock()
    {
        m_spinlock.unlock();
    }

    ScopedSpinlock(const ScopedSpinlock&) = delete;
    ScopedSpinlock& operator=(const ScopedSpinlock&) = delete;

private:
    Spinlock& m_spinlock;
};

class btCollisionDispatcherMt : public btCollisionDispatcher
{
public:
	btCollisionDispatcherMt(btCollisionConfiguration* config, int grainSize = 40);

	virtual btPersistentManifold* getNewManifold(const btCollisionObject* body0, const btCollisionObject* body1) override;
	virtual void releaseManifold(btPersistentManifold* manifold) override;

	virtual void dispatchAllCollisionPairs(btOverlappingPairCache* pairCache, const btDispatcherInfo& info, btDispatcher* dispatcher) override;

protected:
	int m_grainSize;
    Spinlock m_spinLock;
};

#endif  //BT_COLLISION_DISPATCHER_MT_H
