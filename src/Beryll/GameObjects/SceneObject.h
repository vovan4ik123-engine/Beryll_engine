#pragma once

#include "Beryll/Core/GameObject.h"
#include "Beryll/Renderer/Renderer.h"

namespace Beryll
{
    class SceneObject : public GameObject
    {
    public:
        ~SceneObject() override {}

        void setPosition(const glm::vec3& pos, bool resetVelocities = true)
        {
            m_position = pos;

            m_translateMatrix = glm::translate(glm::mat4x4{1.0f}, pos);

            m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;

            if(m_hasCollisionObject)
            {
                Beryll::Physics::setPosition(m_ID, pos, resetVelocities);
            }
        }

        // inherited pure virtual methods here

    protected:
        // for all objects
        glm::mat4 m_MVP{1.0f};
        glm::mat4 m_modelMatrix{1.0f};

        // for objects which has collision object
        glm::mat4 m_scaleMatrix{1.0f};
        glm::mat4 m_rotateMatrix{1.0f};
        glm::mat4 m_translateMatrix{1.0f};
        PhysicsTransforms m_physicsTransforms;
    };
}