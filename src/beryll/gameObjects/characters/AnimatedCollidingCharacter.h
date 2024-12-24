#pragma once

#include "beryll/gameObjects/AnimatedCollidingObject.h"
#include "CharacterController.h"

namespace Beryll
{
    class AnimatedCollidingCharacter : public AnimatedCollidingObject
    {
    public:
        AnimatedCollidingCharacter() = delete;
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
        AnimatedCollidingCharacter(const char* filePath,  // Common params.
                                   float collisionMassKg,    // Physics params.
                                   bool wantCollisionCallBack,
                                   CollisionFlags collFlag,
                                   CollisionGroups collGroup,
                                   CollisionGroups collMask,
                                   SceneObjectGroups sceneGroup);
        ~AnimatedCollidingCharacter() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void setGravity(const glm::vec3& grav, bool resetVelocities = false, bool activate = true) override
        {
            BR_ASSERT((!glm::any(glm::isnan(grav))), "%s", "You want set grav but it is NAN.");

            if(m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC)
            {
                // Gravity will applied by CharacterController.
                SceneObject::setGravity(glm::vec3(0.0f), resetVelocities, activate);

                // Store gravity only inside character.
                m_gravity = grav;
            }
        }

        const glm::vec3 getGravity() const override
        {
            return m_gravity;
        }

        void applyCentralImpulse(const glm::vec3& impulse) const override
        {
            BR_ASSERT(false, "%s", "Dont apply impulse to character. Use or modify character controller.");
        }

        void applyTorqueImpulse(const glm::vec3& impulse) const override
        {
            BR_ASSERT(false, "%s", "Dont apply impulse to character. Use or modify character controller.");
        }

        CharacterController& getController() { return m_controller; }

    private:
        CharacterController m_controller{this};
        glm::vec3 m_gravity{0.0f};
    };
}
