#pragma once

#include "beryll/gameObjects/AnimatedCollidingObject.h"

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

        // Character controller.
        void moveToDirection(glm::vec3 direction, bool rotateWhenMove, bool ignoreYAxisWhenRotate, bool pushDynamicObjects);
        void moveToPosition(const glm::vec3& position, bool rotateWhenMove, bool ignoreYAxisWhenRotate, bool pushDynamicObjects);
        bool jump();
        bool getIsCanStay() { return m_characterCanStay; }
        bool getIsMoving() { return m_characterMoving; }
        bool getIsJumped() { return m_jumped; }
        bool getIsFalling() { return m_falling; }
        bool getIsTouchGroundAfterFall() { return m_touchGroundAfterFall; }
        float getFallDistance() { return m_fallDistance; }

        float getFromOriginToTop() { return m_fromOriginToTop; }
        float getFromOriginToBottom() { return m_fromOriginToBottom; }
        float getXZRadius() { return m_XZRadius; }
        float getHeight() { return m_characterHeight; }

        float moveSpeed = 0.0f; // Calculated in constructor. For average human = 13 km/h = 3m/s.
        float backwardMoveFactor = 0.6f; // Factor to multiply moveSpeed if character move backward.
        float walkableFloorAngleRadians = glm::radians(60.0f);
        float maxStepHeight = 0.0f; // Calculated in constructor.
        float startJumpAngleRadians = glm::radians(25.0f);
        float startJumpPower = 1.0f;
        float startFallingPower = 1.0f; // -Y axis impulse when stat falling.
        float airControlFactor = 0.1f; // Factor to multiply moveSpeed if character not on ground.
        float canStayOrJumpExtendTime = 0.0f; // In seconds. For eliminate cases when player is very close to ground but not touch ground.

    private:
        float m_fromOriginToTop = 0.0f; // Distance between origin and character top.
        float m_fromOriginToBottom = 0.0f; // Distance between origin and character bottom.
        float m_XZRadius = 0.0f; // Radius on X/Z axis. From collision mesh origin.
        float m_characterHeight = 0.0f;

        bool m_characterCanStay = false; // Can stay on any colliding object from group m_collisionMask.
        float m_lastTimeOnGround = 0.0f;
        bool m_characterMoving = false;
        bool m_touchGroundAfterFall = false;

        float m_previousYPos = 0.0f;

        glm::vec3 m_jumpDirection{0.0f, 1.0f, 0.0f};
        bool m_jumped = false;
        bool m_falling = false;
        bool m_startFalling = false;
        float m_startFallingHeight = 0.0f;
        float m_fallDistance = 0.0f;
        bool m_canApplyStartFallingImpulse = false;

        std::vector<int> m_collidingObjects; // Prevent creation and deletion every frame.
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingPoints; // Prevent creation and deletion every frame.

        std::pair<glm::vec3, glm::vec3> m_bottomCollisionPoint; // Lowest collision point with ground ant its normal.

        void move(glm::vec3 moveVector, bool pushDynamicObjects);
    };
}
