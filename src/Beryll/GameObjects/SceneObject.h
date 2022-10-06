#pragma once

#include "Beryll/Core/GameObject.h"
#include "Beryll/Renderer/Renderer.h"
#include "Beryll/Utils/CommonUtils.h"
#include "Beryll/Utils/Matrix.h"

namespace Beryll
{
    enum class MoveDirection
    {
        FORWARD = 0, BACKWARD = 1, RIGHT = 2, LEFT = 3
    };

    class SceneObject : public GameObject
    {
    public:
        ~SceneObject() override {}

        void setOrigin(const glm::vec3& orig, bool resetVelocities = false)
        {
            if(m_origin == orig) { return; }

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Beryll::Physics::setOrigin(m_ID, orig, resetVelocities);
            }
            else
            {
                m_origin = orig;

                m_modelMatrix[3][0] = m_origin.x;
                m_modelMatrix[3][1] = m_origin.y;
                m_modelMatrix[3][2] = m_origin.z;
            }
        }

        void addToOrigin(const glm::vec3& distance, bool resetVelocities = false)
        {
            if(distance.x == 0.0f && distance.y == 0.0f && distance.z == 0.0f) { return; }

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Beryll::Physics::addToOrigin(m_ID, distance, resetVelocities);
            }
            else
            {
                m_origin += distance;

                m_modelMatrix[3][0] = m_origin.x;
                m_modelMatrix[3][1] = m_origin.y;
                m_modelMatrix[3][2] = m_origin.z;
            }
        }

        void setRotation(float angleRad, const glm::vec3& axis, bool resetVelocities = false)
        {
            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Beryll::Physics::setRotation(m_ID, glm::angleAxis(angleRad, axis), resetVelocities);
            }
            else
            {
                m_scaleMatrix = glm::scale(glm::mat4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
                m_rotateMatrix = glm::rotate(glm::mat4{1.0f}, angleRad, axis);
                m_translateMatrix = glm::translate(glm::mat4{1.0f}, Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix));

                m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
            }
        }

        void addToRotation(float angleRad, const glm::vec3& axis, bool resetVelocities = false)
        {
            if(angleRad < 0.0017f) { return; } // less that 0.1 degree

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Physics::addToRotation(m_ID, glm::angleAxis(angleRad, axis), resetVelocities);
            }
            else
            {
                m_modelMatrix = glm::rotate(m_modelMatrix, angleRad, axis);
            }
        }

        void addToRotation(const glm::quat& rot, bool resetVelocities = false)
        {
            if(rot.w == 1.0f) { return; } // no rotation. quaternion musts be unit !!!

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Physics::addToRotation(m_ID, rot, resetVelocities);
            }
            else
            {
                m_scaleMatrix = glm::scale(glm::mat4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
                m_rotateMatrix = glm::toMat4(rot * glm::quat(m_modelMatrix));
                m_translateMatrix = glm::translate(glm::mat4{1.0f}, Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix));

                m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
            }
        }

        void setAngularFactor(const glm::vec3& angFactor, bool resetVelocities = false)
        {
            if(m_angularFactor != angFactor && m_hasCollisionObject)
            {
                Beryll::Physics::setAngularFactor(m_ID, angFactor, resetVelocities);
                m_angularFactor = angFactor;
            }
        }

        void setLinearFactor(const glm::vec3& linFactor, bool resetVelocities = false)
        {
            if(m_linearFactor != linFactor && m_hasCollisionObject)
            {
                Beryll::Physics::setLinearFactor(m_ID, linFactor, resetVelocities);
                m_linearFactor = linFactor;
            }
        }

        void disableGravity(bool resetVelocities = false)
        {
            if(m_gravityEnabled && m_hasCollisionObject)
            {
                Beryll::Physics::disableGravityForObject(m_ID, resetVelocities);
                m_gravityEnabled = false;
            }
        }

        void enableGravity(bool resetVelocities = false)
        {
            if(!m_gravityEnabled && m_hasCollisionObject)
            {
                Beryll::Physics::enableGravityForObject(m_ID, resetVelocities);

                m_gravityEnabled = true;
            }
        }

        bool getIsActive()
        {
            if(m_hasCollisionObject)
            {
                return Physics::getIsObjectActive(m_ID);
            }

            return false;
        }

        void resetVelocities()
        {
            if(m_hasCollisionObject)
            {
                Physics::resetVelocitiesForObject(m_ID);
            }
        }

        void applyImpulse(const glm::vec3& impulse)
        {
            if(m_hasCollisionObject)
            {
                Physics::applyCentralImpulseForObject(m_ID, impulse);
            }
        }

        const glm::mat4& getModelMatrix()
        {
            return m_modelMatrix;
        }

        // inherited pure virtual methods are here

    protected:
        // for all objects
        glm::mat4 m_MVP{1.0f};
        glm::mat4 m_modelMatrix{1.0f};

        glm::mat4 m_scaleMatrix{1.0f};
        glm::mat4 m_rotateMatrix{1.0f};
        glm::mat4 m_translateMatrix{1.0f};
        PhysicsTransforms m_physicsTransforms;

        bool m_gravityEnabled = true;
        glm::vec3 m_linearFactor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_angularFactor = glm::vec3(1.0f, 1.0f, 1.0f);
    };
}