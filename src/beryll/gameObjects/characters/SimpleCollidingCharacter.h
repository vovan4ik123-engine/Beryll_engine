#pragma once

#include "beryll/gameObjects/SimpleCollidingObject.h"
#include "CharacterController.h"

namespace Beryll
{
    class SimpleCollidingCharacter : public SimpleCollidingObject
    {
    public:
        SimpleCollidingCharacter() = delete;
        /*
         * filePath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * collisionMassKg - mass of this object for physics simulation. 0 for static objects
         * wantCollisionCallBack - drop performance too much because call back use std::scoped_lock<std::mutex>
         *                         if true Physics module will store actual collisions for this object,
         *                         you can check it with Physics::getIsCollision(id1, id2) or Physics::getAllCollisions()
         * collFlag - type of collision object in physics world
         * collGroup - group or groups of current object in physics world
         * collMask - should contain collGroup or groups with which you want collisions
         * sceneGroup - game specific group to which this scene object belong
         */
        SimpleCollidingCharacter(const char* filePath,  // Common params.
                                 float collisionMassKg,    // Physics params.
                                 bool wantCollisionCallBack,
                                 CollisionFlags collFlag,
                                 CollisionGroups collGroup,
                                 CollisionGroups collMask,
                                 SceneObjectGroups sceneGroup);
        ~SimpleCollidingCharacter() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        CharacterController& getController() { return m_controller; }

    private:
        CharacterController m_controller{this};
    };
}
