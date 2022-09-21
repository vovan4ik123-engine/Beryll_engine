#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/Physics/Physics.h"
#include "Beryll/Utils/CommonUtils.h"
#include "Beryll/Core/Log.h"

namespace Beryll
{
    // base class of all game objects in game (3d scene and GUI objects)
    // game objects are handled in layers in game state
    class GameObject
    {
    public:
        virtual ~GameObject() {}

        virtual void updateBeforePhysics() = 0; // handle users input, move objects here
        virtual void updateAfterPhysics() = 0; // update positions after simulation, resolve collisions, Physics::getTransforms() here
        virtual void draw() = 0; // subclass graphics
        virtual void playSound() = 0; // subclass sounds

        int getID() { return m_ID; }
        const glm::vec3& getOrigin() { return m_origin; }
        bool getHasCollisionObject() { return m_hasCollisionObject; }
        bool getIsEnabled() { return m_isEnabled; } // use it for disable object from update/draw/sound loops
        CollisionGroups getCollisionGroup() { return m_collisionGroup; }

        void enable()
        {
            if(!m_isEnabled)
            {
                m_isEnabled = true;

                if(m_hasCollisionObject)
                    Beryll::Physics::restoreObject(m_ID);
            }
        }

        void disable()
        {
            if(m_isEnabled)
            {
                m_isEnabled = false;

                if (m_hasCollisionObject)
                    Beryll::Physics::softRemoveObject(m_ID);
            }
        }

    protected:
        const int m_ID = Utils::Common::generateID(); // unique
        glm::vec3 m_origin{0.0f};
        bool m_hasCollisionObject = false; // set true for all collision objects
        CollisionGroups m_collisionGroup = CollisionGroups::NONE; // set inside colliding objects
        bool m_isEnabled = true; // disable object for performance
    };
}
