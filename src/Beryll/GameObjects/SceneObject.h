#pragma once

#include "Beryll/Core/GameObject.h"
#include "Beryll/Utils/CommonUtils.h"
#include "Beryll/Utils/Matrix.h"

namespace Beryll
{
    enum class MoveDirection
    {
        FORWARD = 0, BACKWARD = 1, RIGHT = 2, LEFT = 3
    };

    // this enum game dependent.
    // when you create game you can add here groups and mark objects with them
    // then check these groups in game loop for specific actions/rendering
    enum class SceneObjectGroups
    {
        NONE = 0,
        GROUND = 1,
        PLAYER = 2,
        ENEMY = 3,
        GRASS = 4,
        STONE = 5,
        ROCK = 6,
        BULLET = 7,
        ROOT = 8,
        MUSHROOM = 9,
        WALL = 10,
        TREE = 11,
        TREE_SPRUCE = 12,
        TREE_PINE = 13,

        YOUR_GAME_GROUP = 14
    };

    class SceneObject : public GameObject
    {
    public:
        ~SceneObject() override {}

        void setOrigin(const glm::vec3& orig, bool resetVelocities = false)
        {
            if(m_origin == orig) { return; }

            m_origin = orig;

            m_modelMatrix[3][0] = m_origin.x;
            m_modelMatrix[3][1] = m_origin.y;
            m_modelMatrix[3][2] = m_origin.z;

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Beryll::Physics::setOrigin(m_ID, orig, resetVelocities);
            }
        }

        void addToOrigin(const glm::vec3& distance, bool resetVelocities = false)
        {
            if(distance.x == 0.0f && distance.y == 0.0f && distance.z == 0.0f) { return; }

            m_origin += distance;

            m_modelMatrix[3][0] = m_origin.x;
            m_modelMatrix[3][1] = m_origin.y;
            m_modelMatrix[3][2] = m_origin.z;

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Beryll::Physics::addToOrigin(m_ID, distance, resetVelocities);
            }
        }

        void setRotation(float angleRad, const glm::vec3& axis, bool resetVelocities = false)
        {
            m_scaleMatrix = glm::scale(glm::mat4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
            m_rotateMatrix = glm::rotate(glm::mat4{1.0f}, angleRad, axis);
            m_translateMatrix = glm::translate(glm::mat4{1.0f}, Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix));

            m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Beryll::Physics::setRotation(m_ID, glm::angleAxis(angleRad, axis), resetVelocities);
            }
        }

        void addToRotation(float angleRad, const glm::vec3& axis, bool resetVelocities = false)
        {
            if(angleRad < 0.0017f) { return; } // less that 0.1 degree

            m_modelMatrix = glm::rotate(m_modelMatrix, angleRad, axis);

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Physics::addToRotation(m_ID, glm::angleAxis(angleRad, axis), resetVelocities);
            }
        }

        void addToRotation(const glm::quat& rot, bool resetVelocities = false)
        {
            if(rot.w == 1.0f) { return; } // no rotation. quaternion must be unit !!!

            m_scaleMatrix = glm::scale(glm::mat4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
            m_rotateMatrix = glm::toMat4(rot * glm::quat(m_modelMatrix));
            m_translateMatrix = glm::translate(glm::mat4{1.0f}, Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix));

            m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Physics::addToRotation(m_ID, rot, resetVelocities);
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

        void enableDefaultGravity(bool resetVelocities = false)
        {
            if(!m_gravityEnabled && m_hasCollisionObject)
            {
                Beryll::Physics::enableDefaultGravityForObject(m_ID, resetVelocities);

                m_gravityEnabled = true;
            }
        }

        void setGravity(const glm::vec3& grav, bool resetVelocities = false)
        {
            if(m_gravity != grav && m_hasCollisionObject)
            {
                Beryll::Physics::setGravityForObject(m_ID, grav, resetVelocities);
                m_gravity = grav;
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

        const glm::vec3& getOrigin() { return m_origin; }
        bool getIsEnabledOnScene() { return m_isEnabledOnScene; } // use it for disable object from update/draw/sound loops
        bool getHasCollisionMesh() { return m_hasCollisionObject; }
        bool getIsEnabledCollisionMesh() { return m_isEnabledInPhysicsSimulation; }
        CollisionGroups getCollisionGroup() { return m_collisionGroup; }
        CollisionFlags getCollisionFlag() { return m_collisionFlag; }
        SceneObjectGroups getSceneObjectGroup() { return m_sceneObjectGroup; }

        void enableOnScene()
        {
            m_isEnabledOnScene = true;
        }

        void enableCollisionMesh(bool resetVelocities = false)
        {
            if(m_hasCollisionObject && !m_isEnabledInPhysicsSimulation)
            {
                Beryll::Physics::restoreObject(m_ID, resetVelocities);
                m_isEnabledInPhysicsSimulation = true;
            }
        }

        void disableOnScene()
        {
            m_isEnabledOnScene = false;
        }

        void disableCollisionMesh()
        {
            if (m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
            {
                Beryll::Physics::softRemoveObject(m_ID);
                m_isEnabledInPhysicsSimulation = false;
            }
        }

        /*
         * inherited pure virtual methods are here
         */

        bool useInternalShader = true;
        bool useInternalTextures = true;

    protected:
        glm::mat4 m_MVP{1.0f};
        glm::mat4 m_modelMatrix{1.0f};

        glm::mat4 m_scaleMatrix{1.0f};
        glm::mat4 m_rotateMatrix{1.0f};
        glm::mat4 m_translateMatrix{1.0f};
        PhysicsTransforms m_physicsTransforms;

        bool m_gravityEnabled = true;
        glm::vec3 m_gravity{0.0f};
        glm::vec3 m_linearFactor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_angularFactor = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::vec3 m_origin{0.0f};

        bool m_isEnabledOnScene = true; // disable object for performance

        bool m_hasCollisionObject = false; // set true for all collision objects
        CollisionGroups m_collisionGroup = CollisionGroups::NONE; // set inside colliding objects
        CollisionFlags m_collisionFlag = CollisionFlags::NONE; // set inside colliding objects
        bool m_isEnabledInPhysicsSimulation = false; // set inside colliding objects

        SceneObjectGroups m_sceneObjectGroup = SceneObjectGroups::NONE; // any scene object can belong to specific group
    };
}