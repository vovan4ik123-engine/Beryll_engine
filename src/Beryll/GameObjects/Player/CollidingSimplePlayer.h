#pragma once

#include "Beryll/GameObjects/CollidingSimpleObject.h"

namespace Beryll
{
    class CollidingSimplePlayer : public CollidingSimpleObject
    {
    public:
        CollidingSimplePlayer() = delete;
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
        CollidingSimplePlayer(const char* modelPath,  // common params
                              float collisionMass,    // physics params
                              bool wantCollisionCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask,
                              SceneObjectGroups objGroup = SceneObjectGroups::NONE);
        ~CollidingSimplePlayer() override;

        // methods from base class
        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

        // player controller
        void move(MoveDirection direction);
        void jump();

        float moveSpeed = 13.0f; // meters in second.
        float backwardMoveFactor = 0.6f; // factor to multiply moveSpeed if player move backward
        float walkableFloorAngleRadians = glm::radians(60.0f);
        float maxStepHeight = 2.0f; // in meters. MUST be less than m_playerHeight
        float startJumpAngleRadians = glm::radians(50.0f);
        float startJumpPower = 1.0f;
        float startFallingPower = 1.0f; // -y axis impulse when stat falling
        float airControlFactor = 0.3f; // factor to multiply moveSpeed if player not on ground
        float jumpExtendTime = 0.0f; // in seconds. time when player moved from ground edge to air but still can jump

    private:
        float m_fromOriginToTop = 0.0f; // distance between origin and player top
        float m_fromOriginToBottom = 0.0f; // distance between origin and player bottom
        float m_XZradius = 0.0f; // radius on X/Z axis. from collision mesh origin
        float m_playerHeight = 0.0f;
        float m_playerMass = 0.0f;

        bool m_playerOnGround = false;
        float m_lastTimeOnGround = 0.0f;
        bool m_playerMoving = false;

        float m_previousYPos = 0.0f;

        glm::vec3 m_eyeDirectionXYZ{1.0f, 0.0f, 0.0f}; // start position = looking along +X axis
        glm::vec3 m_eyeDirectionXZ{1.0f, 0.0f, 0.0f};
        glm::vec3 m_backDirectionXZ{-1.0f, 0.0f, 0.0f};
        glm::vec3 m_rightDirectionXZ{0.0f, 0.0f, 0.0f};
        glm::vec3 m_leftDirectionXZ{0.0f, 0.0f, 0.0f};

        glm::vec3 m_jumpDirection{0.0f, 1.0f, 0.0f};
        bool m_jumped = false;
        bool m_falling = false;
        bool m_canApplyStartFallingImpulse = false;

        std::vector<int> m_collidingStaticObjects; // prevent creation and deletion every frame
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingStaticPoints; // prevent creation and deletion every frame

        std::vector<int> m_collidingDynamicObjects; // prevent creation and deletion every frame
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingDynamicPoints; // prevent creation and deletion every frame

        std::pair<glm::vec3, glm::vec3> m_bottomCollisionPoint; // lowest collision point with ground ant its normal
    };
}