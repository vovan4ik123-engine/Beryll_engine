#pragma once

#include "Beryll/GameObjects/SimpleCollidingObject.h"

namespace Beryll
{
    class SimpleCollidingCharacter : public SimpleCollidingObject
    {
    public:
        SimpleCollidingCharacter() = delete;
        /*
         * modelPath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * collisionMass - mass of this object for physics simulation. 0 for static objects
         *                 for example map ground should be alwaus enabled
         * wantCollisionCallBack - if true Physics module will store actual collisions for this object,
         *                         you can check it with Physics::getIsCollision(id1, id2) or Physics::getAllCollisions()
         * collFlag - type of collision object in physics world
         * collGroup - group or groups of current object in physics world
         * collMask - should contain collGroup or groups with which you want collisions
         * objGroup - game specific group to which this scene object belong
         */
        SimpleCollidingCharacter(const char* modelPath,  // common params
                                 float collisionMass,    // physics params
                                 bool wantCollisionCallBack,
                                 CollisionFlags collFlag,
                                 CollisionGroups collGroup,
                                 CollisionGroups collMask,
                                 SceneObjectGroups objGroup = SceneObjectGroups::NONE);
        ~SimpleCollidingCharacter() override;

        // methods from base class
        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        // character controller
        void moveToDirection(glm::vec3 direction);
        void moveToPosition(glm::vec3 position);
        void jump();
        bool getIsCanStay() { return m_characterCanStay; }
        bool getIsMoving() { return m_characterMoving; }
        bool getIsJumped() { return m_jumped; }
        bool getIsFalling() { return m_falling; }

        float getFromOriginToTop() { return m_fromOriginToTop; }
        float getFromOriginToBottom() { return m_fromOriginToBottom; }
        float getXZRadius() { return m_XZRadius; }
        float getHeight() { return m_characterHeight; }

        float moveSpeed = 4.16f; // meters in second
        float backwardMoveFactor = 0.6f; // factor to multiply moveSpeed if character move backward
        float walkableFloorAngleRadians = glm::radians(60.0f);
        float maxStepHeight = 2.0f; // in meters. MUST be less than m_characterHeight
        float startJumpAngleRadians = glm::radians(50.0f);
        float startJumpPower = 1.0f;
        float startFallingPower = 1.0f; // -y axis impulse when stat falling
        float airControlFactor = 0.3f; // factor to multiply moveSpeed if character not on ground
        float jumpExtendTime = 0.0f; // in seconds. time when character moved from ground edge to air but still can jump

    private:
        float m_fromOriginToTop = 0.0f; // distance between origin and character top
        float m_fromOriginToBottom = 0.0f; // distance between origin and character bottom
        float m_XZRadius = 0.0f; // radius on X/Z axis. from collision mesh origin
        float m_characterHeight = 0.0f;
        float m_characterMass = 0.0f;

        bool m_characterCanStay = false; // can stay on any colliding object from group m_collisionMask
        float m_lastTimeCanStay = 0.0f;
        bool m_characterMoving = false;

        float m_previousYPos = 0.0f;

        glm::vec3 m_jumpDirection{0.0f, 1.0f, 0.0f};
        bool m_jumped = false;
        bool m_falling = false;
        bool m_canApplyStartFallingImpulse = false;

        std::vector<int> m_collidingObjects; // prevent creation and deletion every frame
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingPoints; // prevent creation and deletion every frame

        std::pair<glm::vec3, glm::vec3> m_bottomCollisionPoint; // lowest collision point with ground ant its normal
    };
}