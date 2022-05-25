#pragma once

#include "Beryll/Core/GameObject.h"
#include "Beryll/Renderer/Renderer.h"

namespace Beryll
{
    class SceneObject : public GameObject
    {
    public:
        ~SceneObject() override {}

        void setTransforms(const glm::vec3& pos,
                           const glm::quat& rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                           const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f),
                           bool resetVelocities = true)
        {
            m_position = pos;

            m_translateMatrix = glm::translate(glm::mat4x4{1.0f}, pos);
            m_rotateMatrix = glm::toMat4(rot);
            m_scaleMatrix = glm::scale(glm::mat4x4{1.0f}, scale);

            m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;

            if(m_hasCollisionObject)
            {
                Beryll::Physics::setTransforms(m_ID, pos, rot, resetVelocities);
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