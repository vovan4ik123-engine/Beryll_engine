#pragma once

#include "beryll/core/GameObject.h"
#include "beryll/utils/CommonUtils.h"
#include "beryll/utils/Matrix.h"

namespace Beryll
{
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

        PLAYER_MELEE_GROUP_1,
        PLAYER_MELEE_GROUP_2,
        PLAYER_MELEE_GROUP_3,
        PLAYER_MELEE_GROUP_4,
        PLAYER_MELEE_GROUP_5,
        PLAYER_MELEE_GROUP_6,
        PLAYER_MELEE_GROUP_7,
        PLAYER_MELEE_GROUP_8,
        PLAYER_MELEE_GROUP_9,
        PLAYER_MELEE_GROUP_10,
        ENEMY_MELEE_GROUP_1,
        ENEMY_MELEE_GROUP_2,
        ENEMY_MELEE_GROUP_3,
        ENEMY_MELEE_GROUP_4,
        ENEMY_MELEE_GROUP_5,
        ENEMY_MELEE_GROUP_6,
        ENEMY_MELEE_GROUP_7,
        ENEMY_MELEE_GROUP_8,
        ENEMY_MELEE_GROUP_9,
        ENEMY_MELEE_GROUP_10
    };

    class SceneObject : public GameObject
    {
    public:
        ~SceneObject() override {}

        void setOrigin(const glm::vec3& orig, bool resetVelocities = false)
        {
            if(m_origin == orig) { return; }

            m_origin = orig;
            m_translateMatrix = glm::translate(glm::mat4{1.0f}, m_origin);

            m_modelMatrix[3][0] = m_origin.x;
            m_modelMatrix[3][1] = m_origin.y;
            m_modelMatrix[3][2] = m_origin.z;

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Physics::setOrigin(m_ID, orig, resetVelocities);
            }
        }

        void addToOrigin(const glm::vec3& distance, bool resetVelocities = false)
        {
            if(distance.x == 0.0f && distance.y == 0.0f && distance.z == 0.0f) { return; }

            m_origin += distance;
            m_translateMatrix = glm::translate(glm::mat4{1.0f}, m_origin);

            m_modelMatrix[3][0] = m_origin.x;
            m_modelMatrix[3][1] = m_origin.y;
            m_modelMatrix[3][2] = m_origin.z;

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Physics::addToOrigin(m_ID, distance, resetVelocities);
            }
        }

        void addToRotation(float angleRad, const glm::vec3& axis, bool resetVelocities = false)
        {
            if(angleRad < 0.0035f) { return; } // less that 0.2 degree

            m_rotateMatrix = glm::rotate(glm::mat4{1.0f}, angleRad, axis) * m_rotateMatrix;
            // translate and scale matrices should be same
            m_modelMatrix = m_translateMatrix * m_rotateMatrix; // * m_scaleMatrix;

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Physics::addToRotation(m_ID, glm::angleAxis(angleRad, axis), resetVelocities);
            }
        }

        void addToRotation(const glm::quat& qua, bool resetVelocities = false)
        {
            if(glm::angle(qua) < 0.0035f) { return; } // less that 0.2 degree

            m_rotateMatrix = glm::toMat4(qua) * m_rotateMatrix;
            // translate and scale matrices should be same
            m_modelMatrix = m_translateMatrix * m_rotateMatrix; // * m_scaleMatrix;

            if(m_hasCollisionObject)
            {
                // game object with collision object will take transforms from physics module after simulation
                Physics::addToRotation(m_ID, qua, resetVelocities);
            }
        }

        void rotateToPoint(const glm::vec3 point, bool ignoreYAxisWhenRotate)
        {
            rotateToDirection(point - m_origin, ignoreYAxisWhenRotate);
        }

        void rotateToDirection(const glm::vec3 dir, bool ignoreYAxisWhenRotate)
        {
            if(dir.x == 0.0f && dir.y == 0.0f && dir.z == 0.0f) { return; }

            if(ignoreYAxisWhenRotate)
            {
                glm::vec3 dirXZ = glm::normalize(glm::vec3{dir.x, 0.0f, dir.z});
                addToRotation(glm::rotation(getFaceDirXZ(), dirXZ));
            }
            else
            {
                addToRotation(glm::rotation(getFaceDirXYZ(), glm::normalize(dir)));
            }
        }

        void setAngularFactor(const glm::vec3& angFactor, bool resetVelocities = false)
        {
            if(m_angularFactor != angFactor && m_hasCollisionObject)
            {
                Physics::setAngularFactor(m_ID, angFactor, resetVelocities);
                m_angularFactor = angFactor;
            }
        }

        void setLinearFactor(const glm::vec3& linFactor, bool resetVelocities = false)
        {
            if(m_linearFactor != linFactor && m_hasCollisionObject)
            {
                Physics::setLinearFactor(m_ID, linFactor, resetVelocities);
                m_linearFactor = linFactor;
            }
        }

        void disableGravity(bool resetVelocities = false)
        {
            if(m_gravityEnabled && m_hasCollisionObject)
            {
                Physics::disableGravityForObject(m_ID, resetVelocities);
                m_gravityEnabled = false;
            }
        }

        void enableDefaultGravity(bool resetVelocities = false)
        {
            if(!m_gravityEnabled && m_hasCollisionObject)
            {
                Physics::enableDefaultGravityForObject(m_ID, resetVelocities);
                m_gravityEnabled = true;
            }
        }

        void setGravity(const glm::vec3& grav, bool resetVelocities = false)
        {
            if(m_gravity != grav && m_hasCollisionObject)
            {
                Physics::setGravityForObject(m_ID, grav, resetVelocities);
                m_gravity = grav;
            }
        }

        void activate()
        {
            if(m_hasCollisionObject)
            {
                Physics::activateObject(m_ID);
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
        bool getIsDisabledForEver() { return m_disabledForEver; } // use it for disable object from draw
        bool getIsEnabledDraw() { return m_isEnabledDraw && !m_disabledForEver; } // use it for disable object from draw
        bool getIsEnabledUpdate() { return m_isEnabledUpdate && !m_disabledForEver; } // use it for disable object from updates
        bool getHasCollisionMesh() { return m_hasCollisionObject; }
        bool getIsEnabledCollisionMesh() { return m_isEnabledInPhysicsSimulation && !m_disabledForEver; }
        CollisionGroups getCollisionGroup() { return m_collisionGroup; }
        CollisionFlags getCollisionFlag() { return m_collisionFlag; }
        SceneObjectGroups getSceneObjectGroup() { return m_sceneObjectGroup; }
        glm::vec3 getFaceDirXYZ()
        {
            return glm::normalize(glm::vec3(m_rotateMatrix * glm::vec4(m_sceneObjectFaceDir, 1.0f)));
        }
        glm::vec3 getFaceDirXZ()
        {
            glm::vec3 dirXYZ = getFaceDirXYZ();
            return glm::normalize(glm::vec3{dirXYZ.x, 0.0f, dirXYZ.z});
        }
        glm::vec3 getRightDirXYZ()
        {
            return glm::normalize(glm::vec3(m_rotateMatrix * glm::vec4(m_sceneObjectRightDir, 1.0f)));
        }
        glm::vec3 getRightDirXZ()
        {
            glm::vec3 dirXYZ = getRightDirXYZ();
            return glm::normalize(glm::vec3{dirXYZ.x, 0.0f, dirXYZ.z});
        }
        glm::vec3 getUpDirXYZ()
        {
            return glm::normalize(glm::vec3(m_rotateMatrix * glm::vec4(m_sceneObjectUpDir, 1.0f)));
        }

        void enableDraw()
        {
            m_isEnabledDraw = !m_disabledForEver;
        }

        void enableUpdate()
        {
            m_isEnabledUpdate = !m_disabledForEver;
        }

        void enableCollisionMesh(bool resetVelocities = false)
        {
            if(m_hasCollisionObject && !m_isEnabledInPhysicsSimulation && !m_disabledForEver)
            {
                Physics::restoreObject(m_ID, resetVelocities);
                m_isEnabledInPhysicsSimulation = true;
            }
        }

        void disableDraw()
        {
            m_isEnabledDraw = false;
        }

        void disableUpdate()
        {
            m_isEnabledUpdate = false;
        }

        void disableCollisionMesh()
        {
            if (m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
            {
                Physics::softRemoveObject(m_ID);
                m_isEnabledInPhysicsSimulation = false;
            }
        }

        void disableForEver()
        {
            m_disabledForEver = true;

            disableDraw();
            disableUpdate();
            disableCollisionMesh();
        }

        /*
         * inherited pure virtual methods are here
         */

        bool useInternalShader = true;
        bool useInternalTextures = true;

    protected:
        glm::vec3 m_origin{0.0f, 0.0f, 0.0f};

        glm::mat4 m_scaleMatrix{1.0f};
        glm::mat4 m_rotateMatrix{1.0f};
        glm::mat4 m_translateMatrix{1.0f};

        glm::mat4 m_modelMatrix{1.0f};
        glm::mat4 m_MVP{1.0f};

        PhysicsTransforms m_physicsTransforms;

        bool m_hasCollisionObject = false; // set true for all collision objects
        CollisionGroups m_collisionGroup = CollisionGroups::NONE; // set inside colliding objects
        CollisionGroups m_collisionMask = CollisionGroups::NONE; // set inside colliding objects
        CollisionFlags m_collisionFlag = CollisionFlags::NONE; // set inside colliding objects
        bool m_isEnabledInPhysicsSimulation = false; // set inside colliding objects

        SceneObjectGroups m_sceneObjectGroup = SceneObjectGroups::NONE; // any scene object can belong to specific group

    private:
        // only for internal checks inside this file
        bool m_gravityEnabled = true;
        glm::vec3 m_gravity{0.0f, -9999.0f, 0.0f};
        glm::vec3 m_linearFactor{1.0f, 1.0f, 1.0f};
        glm::vec3 m_angularFactor{1.0f, 1.0f, 1.0f};

        bool m_disabledForEver = false;
        bool m_isEnabledDraw = true; // for method draw()
        bool m_isEnabledUpdate = true; // for methods updateBeforePhysics() + updateAfterPhysics()

        // If objects on scene has face it should be created in Blender with face directed along +X axis.
        // Engine assume you create 3D models in tool where up axis is +Z (like Blender)
        // and during exporting model you change axis to: up +Y, forward -Z.
        // That will add rotation to exported model: 90 degrees around -X axis or 270 degrees around +X axis
        // If you want take models faceDir or upDir just multiply these vectors by m_rotateMatrix
        const glm::vec3 m_sceneObjectFaceDir{1.0f, 0.0f, 0.0f};
        const glm::vec3 m_sceneObjectRightDir{0.0f, -1.0f, 0.0f};
        const glm::vec3 m_sceneObjectUpDir{0.0f, 0.0f, 1.0f};
    };
}