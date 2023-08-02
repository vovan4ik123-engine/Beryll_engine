#pragma once

#include "beryll/core/GameObject.h"
#include "beryll/utils/CommonUtils.h"
#include "beryll/utils/Matrix.h"

namespace Beryll
{
    // This enum game dependent.
    // When you create game you can add here groups and mark objects with them
    // then check these groups in game loop for specific actions/rendering.
    enum class SceneObjectGroups
    {
        NONE = 0,
        GROUND = 1,
        PLAYER = 2,
        ENEMY = 3,
        BULLET = 4,

        PLAYER_GROUP_1,
        PLAYER_GROUP_2,
        PLAYER_GROUP_3,
        PLAYER_GROUP_4,
        PLAYER_GROUP_5,
        PLAYER_GROUP_6,
        PLAYER_GROUP_7,
        PLAYER_GROUP_8,
        PLAYER_GROUP_9,
        PLAYER_GROUP_10,
        ENEMY_GROUP_1,
        ENEMY_GROUP_2,
        ENEMY_GROUP_3,
        ENEMY_GROUP_4,
        ENEMY_GROUP_5,
        ENEMY_GROUP_6,
        ENEMY_GROUP_7,
        ENEMY_GROUP_8,
        ENEMY_GROUP_9,
        ENEMY_GROUP_10
    };

    class SceneObject : public GameObject
    {
    public:
        ~SceneObject() override {}

        void setOrigin(const glm::vec3& orig, bool resetVelocities = false)
        {
            if(m_origin == orig) { return; }

            m_origin = orig;
            m_originX = m_origin.x;
            m_originY = m_origin.y;
            m_originZ = m_origin.z;

            if(m_hasCollisionObject)
            {
                // Game object with collision object will take transforms from physics module after simulation.
                Physics::setOrigin(m_ID, orig, resetVelocities);
            }
        }

        void addToOrigin(const glm::vec3& distance, bool resetVelocities = false)
        {
            m_origin += distance;
            m_originX = m_origin.x;
            m_originY = m_origin.y;
            m_originZ = m_origin.z;

            if(m_hasCollisionObject)
            {
                // Game object with collision object will take transforms from physics module after simulation.
                Physics::addToOrigin(m_ID, distance, resetVelocities);
            }
        }

        void addToRotation(float angleRad, const glm::vec3& axis, bool resetVelocities = false)
        {
            if(angleRad < 0.0035f) { return; } // less that 0.2 degree.

            glm::quat normQuat = glm::normalize(glm::angleAxis(angleRad, glm::normalize(axis)));

            m_engineAddedRotation = glm::normalize(normQuat * m_engineAddedRotation);

            if(m_hasCollisionObject)
            {
                // Game object with collision object will take transforms from physics module after simulation.
                Physics::addToRotation(m_ID, normQuat, resetVelocities);
            }
        }

        void addToRotation(const glm::quat& qua, bool resetVelocities = false)
        {
            glm::quat normQuat = glm::normalize(qua);

            if(glm::angle(normQuat) < 0.0035f) { return; } // Less than 0.2 degree.

            m_engineAddedRotation = glm::normalize(normQuat * m_engineAddedRotation);

            if(m_hasCollisionObject)
            {
                // Game object with collision object will take transforms from physics module after simulation.
                Physics::addToRotation(m_ID, normQuat, resetVelocities);
            }
        }

        void rotateToPoint(const glm::vec3& point, bool ignoreYAxisWhenRotate)
        {
            rotateToDirection(point - m_origin, ignoreYAxisWhenRotate);
        }

        void rotateToDirection(const glm::vec3& dir, bool ignoreYAxisWhenRotate)
        {
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

        glm::mat4 getModelMatrix(bool includeBlenderFileRotation = true)
        {
            // translate * rotate * scale m_engineAddedRotation
            if(includeBlenderFileRotation)
                return glm::translate(glm::mat4{1.0f}, m_origin) * glm::toMat4(glm::normalize(m_engineAddedRotation * m_originalRotationFromBlenderFile));
            else
                return glm::translate(glm::mat4{1.0f}, m_origin) * glm::toMat4(m_engineAddedRotation);
        }

        glm::vec3 getOrigin()
        {
            float x = m_originX;
            float y = m_originY;
            float z = m_originZ;

            return glm::vec3{x, y, z};
        }
        bool getIsDisabledForEver() { return m_isDisabledForEver; } // use it for disable object from draw
        bool getIsEnabledDraw() { return m_isEnabledDraw && !m_isDisabledForEver; } // use it for disable object from draw
        bool getIsEnabledUpdate() { return m_isEnabledUpdate && !m_isDisabledForEver; } // use it for disable object from updates
        bool getHasCollisionMesh() { return m_hasCollisionObject; }
        bool getIsEnabledCollisionMesh() { return m_isEnabledInPhysicsSimulation && !m_isDisabledForEver; }
        CollisionGroups getCollisionGroup() { return m_collisionGroup; }
        CollisionFlags getCollisionFlag() { return m_collisionFlag; }
        SceneObjectGroups getSceneObjectGroup() { return m_sceneObjectGroup; }
        glm::vec3 getFaceDirXYZ()
        {
            return glm::normalize(glm::vec3(glm::toMat4(glm::normalize(m_engineAddedRotation * m_originalRotationFromBlenderFile)) * glm::vec4(m_sceneObjectFaceDir, 1.0f)));
        }
        glm::vec3 getFaceDirXZ()
        {
            glm::vec3 dirXYZ = getFaceDirXYZ();
            return glm::normalize(glm::vec3{dirXYZ.x, 0.0f, dirXYZ.z});
        }
        glm::vec3 getRightDirXYZ()
        {
            return glm::normalize(glm::vec3(glm::toMat4(glm::normalize(m_engineAddedRotation * m_originalRotationFromBlenderFile)) * glm::vec4(m_sceneObjectRightDir, 1.0f)));
        }
        glm::vec3 getRightDirXZ()
        {
            glm::vec3 dirXYZ = getRightDirXYZ();
            return glm::normalize(glm::vec3{dirXYZ.x, 0.0f, dirXYZ.z});
        }
        glm::vec3 getUpDirXYZ()
        {
            return glm::normalize(glm::vec3(glm::toMat4(glm::normalize(m_engineAddedRotation * m_originalRotationFromBlenderFile)) * glm::vec4(m_sceneObjectUpDir, 1.0f)));
        }

        void enableDraw()
        {
            m_isEnabledDraw = !m_isDisabledForEver;
        }

        void enableUpdate()
        {
            m_isEnabledUpdate = !m_isDisabledForEver;
        }

        void enableCollisionMesh(bool resetVelocities = false)
        {
            if(m_hasCollisionObject && !m_isEnabledInPhysicsSimulation && !m_isDisabledForEver)
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
            m_isDisabledForEver = true;

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
        //float m_scale = 1.0f; // Unused for now.
        glm::quat m_originalRotationFromBlenderFile{1.0f, 0.0f, 0.0f, 0.0f}; // Loaded with model from blender exported file.
        glm::quat m_engineAddedRotation{1.0f, 0.0f, 0.0f, 0.0f}; // Rotation added by user to model.
        glm::vec3 m_origin{0.0f, 0.0f, 0.0f};
        // For synchronization when one thread set origin and other calls getOrigin()
        // for same object.
        std::atomic<float> m_originX = 0.0f;
        std::atomic<float> m_originY = 0.0f;
        std::atomic<float> m_originZ = 0.0f;

        glm::mat4 m_MVP{1.0f};
        PhysicsTransforms m_physicsTransforms;

        bool m_hasCollisionObject = false; // set true for all collision objects
        CollisionGroups m_collisionGroup = CollisionGroups::NONE; // set inside colliding objects
        CollisionGroups m_collisionMask = CollisionGroups::NONE; // set inside colliding objects
        CollisionFlags m_collisionFlag = CollisionFlags::NONE; // set inside colliding objects
        bool m_isEnabledInPhysicsSimulation = false; // set inside colliding objects

        SceneObjectGroups m_sceneObjectGroup = SceneObjectGroups::NONE; // any scene object can belong to specific group

    private:
        // Only for internal checks inside this file.
        bool m_gravityEnabled = true;
        glm::vec3 m_gravity{0.0f, -9999.0f, 0.0f};
        glm::vec3 m_linearFactor{1.0f, 1.0f, 1.0f};
        glm::vec3 m_angularFactor{1.0f, 1.0f, 1.0f};

        bool m_isDisabledForEver = false;
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