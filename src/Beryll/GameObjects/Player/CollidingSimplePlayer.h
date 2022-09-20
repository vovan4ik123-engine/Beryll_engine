#pragma once

#include "Beryll/GameObjects/CollidingSimpleObject.h"

namespace Beryll
{
    class CollidingSimplePlayer : public CollidingSimpleObject
    {
    public:
        CollidingSimplePlayer() = delete;
        /*
         * modelPath - path to models file (.DAE or .FBX). start path from first folder inside assets/
         * collisionMass - mass of this object for physics simulation. 0 for static objects
         *                 for example map ground should be alwaus enabled
         * wantCollisionCallBack - if true Physics module will store actual collisions for this object,
         *                         you can check it with Physics::getIsCollision(id1, id2) or Physics::getAllCollisions()
         * collFlag - type of collision object in physics world
         * collGroup - group or groups of current object in physics world
         * collMask - should contain collGroup or groups with which you want collisions
         * vertexPath - path to vertex shader for this model
         * fragmentPath - path to fragment shader for this model
         * diffSampler - name of sampler2D .... for diffuse texture in fragment shader
         *               sampler2D .... for diffuse texture MUST be first in shader
         * specSampler - name of sampler2D .... for specular texture in fragment shader
         *               sampler2D .... for specular texture MUST be second in shader
         */
        CollidingSimplePlayer(const char* modelPath,  // common params
                              float collisionMass,    // physics params
                              bool wantCollisionCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask,
                              const char* vertexPath, // graphics params
                              const char* fragmentPath,
                              const char* diffSampler,
                              const char* specSampler = nullptr);
        ~CollidingSimplePlayer() override;

        // methods from base class
        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

        // player controller
        void move(MoveDirection direction);
        void jump();

        float moveSpeed = 5.0f; // meter in second
        float walkableFloorAngleRadians = glm::radians(60.0f);
        float startJumpAngleRadians = glm::radians(50.0f);
        float maxStepHeight = 0.2f; // meters
        float airControlFactor = 0.3f; // factor to multiply moveSpeed if player not on ground
        float backwardMoveFactor = 0.6f; // factor to multiply if player move backward

    private:
        float m_fromOriginToBottom = 0.0f; // distance between origin and player bottom
        float m_fromOriginToTop = 0.0f; // distance between origin and player top
        float m_XZradius = 0.0f; // radius on X/Z axis. from collision mesh origin
        float m_playerHeight = 0.0f;
        bool m_playerOnGround = false;
        bool m_playerMoving = false;

        glm::vec3 m_eyeDirectionXYZ{1.0f, 0.0f, 0.0f}; // start position = looking along +X axis
        glm::vec3 m_eyeDirectionXZ{1.0f, 0.0f, 0.0f};
        glm::vec3 m_backDirectionXZ{-1.0f, 0.0f, 0.0f};
        glm::vec3 m_rightDirectionXZ{0.0f, 0.0f, 0.0f};
        glm::vec3 m_leftDirectionXZ{0.0f, 0.0f, 0.0f};

        glm::vec3 m_jumpDirection{0.0f, 1.0f, 0.0f};

        std::vector<int> m_collidingStaticObjects; // prevent creation and deletion every frame
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingStaticPoints; // prevent creation and deletion every frame

        std::vector<int> m_collidingDynamicObjects; // prevent creation and deletion every frame
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingDynamicPoints; // prevent creation and deletion every frame

        std::pair<glm::vec3, glm::vec3> m_bottomCollisionPoint; // lowest collision point with ground ant its normal
    };
}