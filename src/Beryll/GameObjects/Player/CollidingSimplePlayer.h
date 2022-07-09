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
         * canBeDisabled - true if object can be disabled from game (update/draw/playsound/simulate physics)
         *                 object can be dissabled if camera dont see it or distance is too far. For performance
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
                              bool canBeDisabled,
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

    private:
        float m_fromOriginToBottom = 0.0f; // distance between origin and player bottom
        float m_fromOriginToTop = 0.0f; // distance between origin and player top
        float m_XZradius = 0.0f; // radius on X/Z axis. from collision mesh origin

        float m_playerHeight = 0.0f;

        glm::vec3 m_eyeDirection_X_Y_Z{0.0f};
        glm::vec3 m_eyeDirection_X_Z{0.0f};

        glm::vec3 m_moveDirection_X_Y_Z{0.0f};
        glm::vec3 m_moveDirection_X_Z{0.0f};

        std::vector<int> m_collidingObjectsIDs; // prevent creation and deletion every frame!!!
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingPoints; // prevent creation and deletion every frame!!!
    };
}