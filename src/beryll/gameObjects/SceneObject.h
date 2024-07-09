#pragma once

#include "beryll/core/GameObject.h"
#include "beryll/utils/Matrix.h"
#include "beryll/physics/Physics.h"
#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"

namespace Beryll
{
    // This enum is game dependent.
    // When you create game you can add here groups and mark objects with them
    // then check these groups in game loop for specific actions/rendering.
    enum class SceneObjectGroups
    {
        NONE = 0,
        GROUND = 1,
        PLAYER = 2,
        ENEMY = 3,
        BULLET = 4,
        STATIC_ENVIRONMENT = 5,
        DYNAMIC_ENVIRONMENT = 6,
        BALL = 7,
        BUILDING = 8,
        GARBAGE,
        JUMPPAD,
        BOSS
    };

    class SceneObject : public GameObject
    {
    public:
        ~SceneObject() override {}

        void setOrigin(const glm::vec3& orig, bool resetVelocities = false)
        {
            BR_ASSERT((!glm::any(glm::isnan(orig))), "%s", "You want set orig but it is NAN.");

            if(m_origin == orig) { return; }

            m_origin = orig;

            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
                Physics::setOrigin(m_ID, orig, resetVelocities);
        }

        void addToOrigin(const glm::vec3& distance, bool resetVelocities = false)
        {
            BR_ASSERT((!glm::any(glm::isnan(distance))), "%s", "You want add distance but it is NAN.");

            m_origin += distance;

            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
                Physics::addToOrigin(m_ID, distance, resetVelocities);
        }

        void addToRotation(float angleRad, const glm::vec3& axis, bool resetVelocities = false)
        {
            if(angleRad < 0.0017f) { return; } // Less that 0.1 degree.

            glm::quat normQuat = glm::normalize(glm::angleAxis(angleRad, glm::normalize(axis)));

            BR_ASSERT((!glm::any(glm::isnan(normQuat))), "%s", "You want add normQuat but it is NAN.");

            m_totalRotation = glm::normalize(normQuat * m_totalRotation);
            m_engineAddedRotation = glm::normalize(normQuat * m_engineAddedRotation);

            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
                Physics::addToRotation(m_ID, normQuat, resetVelocities);
        }

        void addToRotation(const glm::quat& qua, bool resetVelocities = false)
        {
            glm::quat normQuat = glm::normalize(qua);

            BR_ASSERT((!glm::any(glm::isnan(normQuat))), "%s", "You want add normQuat but it is NAN.");

            if(glm::angle(normQuat) < 0.0017f) { return; } // Less than 0.1 degree.

            m_totalRotation = glm::normalize(normQuat * m_totalRotation);
            m_engineAddedRotation = glm::normalize(normQuat * m_engineAddedRotation);

            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
                Physics::addToRotation(m_ID, normQuat, resetVelocities);
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
                if(glm::any(glm::isnan(dirXZ)) || glm::length(dirXZ) == 0.0f)
                    return;

                addToRotation(glm::rotation(getFaceDirXZ(), dirXZ));
            }
            else
            {
                if(glm::any(glm::isnan(dir)) || glm::length(dir) == 0.0f)
                    return;

                addToRotation(glm::rotation(getFaceDirXYZ(), glm::normalize(dir)));
            }
        }

        void setAngularFactor(const glm::vec3& angFactor, bool resetVelocities = false)
        {
            BR_ASSERT((!glm::any(glm::isnan(angFactor))), "%s", "You want set angFactor but it is NAN.");

            if(m_angularFactor != angFactor && m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC)
            {
                m_angularFactor = angFactor;
                Physics::setAngularFactor(m_ID, angFactor, resetVelocities);
            }
        }

        const glm::vec3& getAngularFactor() const
        {
            BR_ASSERT((m_hasCollisionObject == true && m_collisionFlag == CollisionFlags::DYNAMIC),
                      "%s", "getAngularFactor() should be called only for object with DYNAMIC collider.");
            return m_angularFactor;
        }

        void setLinearFactor(const glm::vec3& linFactor, bool resetVelocities = false)
        {
            BR_ASSERT((!glm::any(glm::isnan(linFactor))), "%s", "You want set linFactor but it is NAN.");

            if(m_linearFactor != linFactor && m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC)
            {
                m_linearFactor = linFactor;
                Physics::setLinearFactor(m_ID, linFactor, resetVelocities);
            }
        }

        const glm::vec3& getLinearFactor() const
        {
            BR_ASSERT((m_hasCollisionObject == true && m_collisionFlag == CollisionFlags::DYNAMIC),
                      "%s", "getLinearFactor() should be called only for object with DYNAMIC collider.");
            return m_linearFactor;
        }

        void setAngularVelocity(const glm::vec3& angVelocity)
        {
            BR_ASSERT((!glm::any(glm::isnan(angVelocity))), "%s", "You want set angVelocity but it is NAN.");

            if(m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC && m_isEnabledInPhysicsSimulation)
            {
                Physics::setAngularVelocity(m_ID, angVelocity);
            }
        }

        glm::vec3 getAngularVelocity() const
        {
            glm::vec3 veloc{0.0f};
            if(m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC && m_isEnabledInPhysicsSimulation)
            {
                veloc = Physics::getAngularVelocity(m_ID);
            }

            return veloc;
        }

        void setLinearVelocity(const glm::vec3& linVelocity)
        {
            BR_ASSERT((!glm::any(glm::isnan(linVelocity))), "%s", "You want set linVelocity but it is NAN.");

            if(m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC && m_isEnabledInPhysicsSimulation)
            {
                Physics::setLinearVelocity(m_ID, linVelocity);
            }
        }

        glm::vec3 getLinearVelocity() const
        {
            glm::vec3 veloc{0.0f};
            if(m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC && m_isEnabledInPhysicsSimulation)
            {
                veloc = Physics::getLinearVelocity(m_ID);
            }

            return veloc;
        }

        void setDefaultGravity(bool resetVelocities = false) const
        {
            if(m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC)
            {
                Physics::setDefaultGravityForObject(m_ID, resetVelocities);
            }
        }

        void setGravity(const glm::vec3& grav, bool resetVelocities = false, bool activate = true)
        {
            BR_ASSERT((!glm::any(glm::isnan(grav))), "%s", "You want set grav but it is NAN.");

            if(m_gravity != grav && m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC)
            {
                //BR_INFO("Set gravity: %f", grav.y);
                m_gravity = grav;
                Physics::setGravityForObject(m_ID, grav, resetVelocities, activate);
            }
        }

        const glm::vec3 getGravity() const
        {
            BR_ASSERT((m_hasCollisionObject == true && m_collisionFlag == CollisionFlags::DYNAMIC),
                      "%s", "getGravity() should be called only for object with DYNAMIC collider.");
            return Physics::getGravityObject(m_ID);
        }

        const float getCollisionMass() const
        {
            BR_ASSERT((m_hasCollisionObject == true && m_collisionFlag == CollisionFlags::DYNAMIC),
                      "%s", "getCollisionMass() should be called only for object with DYNAMIC collider.");
            return m_collisionMass;
        }

        void activate(bool resetVelocities = false) const
        {
            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
            {
                Physics::activateObject(m_ID, resetVelocities);
            }
        }

        void deActivate(bool resetVelocities = false) const
        {
            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
            {
                Physics::deActivateObject(m_ID, resetVelocities);
            }
        }

        const bool getIsActive() const
        {
            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
            {
                return Physics::getIsObjectActive(m_ID);
            }

            return false;
        }

        void resetVelocities() const
        {
            if(m_hasCollisionObject)
            {
                Physics::resetVelocitiesForObject(m_ID);
            }
        }

        void applyCentralImpulse(const glm::vec3& impulse) const
        {
            BR_ASSERT((!glm::any(glm::isnan(impulse))), "%s", "You want set impulse but it is NAN.");

            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
            {
                Physics::applyCentralImpulseForObject(m_ID, impulse);
            }
        }

        void applyTorqueImpulse(const glm::vec3& impulse) const
        {
            BR_ASSERT((!glm::any(glm::isnan(impulse))), "%s", "You want set impulse but it is NAN.");

            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
            {
                Physics::applyTorqueImpulseForObject(m_ID, impulse);
            }
        }

        void setFriction(const float friction) const
        {
            if(m_hasCollisionObject)
            {
                Physics::setFriction(m_ID, friction);
            }
        }

        void setDamping(const float linDamping, const float angDamping)
        {
            if((m_linearDamping != linDamping || m_angularDamping != angDamping) &&
                m_hasCollisionObject && m_collisionFlag == CollisionFlags::DYNAMIC)
            {
                m_linearDamping = linDamping;
                m_angularDamping = angDamping;
                Physics::setDamping(m_ID, linDamping, angDamping);
            }
        }

        const glm::mat4 getModelMatrix(bool includeTotalRotation = true) const
        {
            // modelMatrix = translate * rotate * scale.
            if(includeTotalRotation)
            {
                glm::mat4 modelMatrix = glm::toMat4(m_totalRotation);
                modelMatrix[3][0] = m_origin.x;
                modelMatrix[3][1] = m_origin.y;
                modelMatrix[3][2] = m_origin.z;
                return modelMatrix;
            }
            else
            {
                glm::mat4 modelMatrix = glm::toMat4(m_engineAddedRotation);
                modelMatrix[3][0] = m_origin.x;
                modelMatrix[3][1] = m_origin.y;
                modelMatrix[3][2] = m_origin.z;
                return modelMatrix;
            }
        }

        // Only for one thread.
        const glm::vec3& getOrigin() const
        {
            return m_origin;
        }

        // Use it if you call set/get origin from many threads.
//        glm::vec3 getOriginForMultithreading()
//        {
//            float x = m_originX;
//            float y = m_originY;
//            float z = m_originZ;
//
//            return glm::vec3{x, y, z};
//        }
        const bool getIsDisabledForEver() const { return m_isDisabledForEver; } // Use it for avoid object updating and drawing.
        const bool getIsEnabledDraw() const { return m_isEnabledDraw; } // Use it for avoid object from drawing.
        const bool getIsEnabledUpdate() const { return m_isEnabledUpdate; } // Use it for avoid object from updating.
        const bool getHasCollisionMesh() const { return m_hasCollisionObject; }
        const bool getIsEnabledCollisionMesh() const { return m_isEnabledInPhysicsSimulation; }
        const CollisionGroups getCollisionGroup() const { return m_collisionGroup; }
        const CollisionGroups getCollisionMask() const { return m_collisionMask; }
        const CollisionFlags getCollisionFlag() const { return m_collisionFlag; }
        const SceneObjectGroups getSceneObjectGroup() const { return m_sceneObjectGroup; }
        const bool getIsAnimatedObject() const { return m_isAnimatedObject; }
        const glm::vec3 getFaceDirXYZ() const
        {
            return glm::normalize(glm::vec3(m_totalRotation * m_sceneObjectFaceDir));
        }
        const glm::vec3 getFaceDirXZ() const
        {
            const glm::vec3 dirXYZ = getFaceDirXYZ();
            return glm::normalize(glm::vec3{dirXYZ.x, 0.0f, dirXYZ.z});
        }
        const glm::vec3 getRightDirXYZ() const
        {
            return glm::normalize(glm::vec3(m_totalRotation * m_sceneObjectRightDir));
        }
        const glm::vec3 getRightDirXZ() const
        {
            const glm::vec3 dirXYZ = getRightDirXYZ();
            return glm::normalize(glm::vec3{dirXYZ.x, 0.0f, dirXYZ.z});
        }
        const glm::vec3 getUpDirXYZ() const
        {
            return glm::normalize(glm::vec3(m_totalRotation * m_sceneObjectUpDir));
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
            if(m_hasCollisionObject && m_isEnabledInPhysicsSimulation)
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

        // Returns 0 for objects without collider.
        float getFromOriginToTop() { return m_fromOriginToTop; }
        float getFromOriginToBottom() { return m_fromOriginToBottom; }
        float getXZRadius() { return m_XZRadius; }
        float getObjectHeight() { return m_objectHeight; }

        /*
         * Inherited and new pure virtual methods are here.
         */
        void addMaterial2(const std::string& diffusePath, const std::string& specularPath,
                          const std::string& normalMapPath, const std::string& blendTexturePath);

        const float getAddToUVCoords() const { return m_addToUVCoords; }
        const float getUVCoordsMultiplier() const { return m_UVCoordsMultiplier; }

        bool useInternalShader = true;
        bool useInternalMaterials = true;

    protected:
        //float m_scale = 1.0f; // Unused for now.

        // Total rotation include all rotations:
        //     loaded with model from Blender exported file,
        //     added by physics simulation,
        //     added by engine methods addToRotation(...).
        glm::quat m_totalRotation{1.0f, 0.0f, 0.0f, 0.0f};
        // Only sum of rotations added by engine methods addToRotation(...).
        glm::quat m_engineAddedRotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 m_origin{0.0f, 0.0f, 0.0f};
        // std::atomic for synchronization when one thread set origin and other calls getOriginForMultithreading() for same object.
        //std::atomic<float> m_originX = 0.0f; Assign it in setOrigin()/addToOrigin() if you need getOriginForMultithreading().
        //std::atomic<float> m_originY = 0.0f;
        //std::atomic<float> m_originZ = 0.0f;

        SceneObjectGroups m_sceneObjectGroup = SceneObjectGroups::NONE; // Any scene object can belong to specific group.
        bool m_isAnimatedObject = false;

        // Graphics data.
        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_vertexTangentsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<VertexBuffer> m_boneIDsBuffer;     // Only for animated objects.
        std::shared_ptr<VertexBuffer> m_boneWeightsBuffer; // Only for animated objects.
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader.
        Material1 m_material1;
        std::optional<Material2> m_material2;
        // Can be used in shader to return UV coords in range 0...1 if was scaled. Useful if we have m_material2 with blend texture.
        // Shader code example: vec2 blendTextureUV = (inUV + m_addToUVCoords) * m_UVCoordsMultiplier;
        float m_addToUVCoords = 0.0f;
        float m_UVCoordsMultiplier = 0.0f;
        // Graphics data end.

        // Collider data.
        float m_smallestX = std::numeric_limits<float>::max();
        float m_biggestX = std::numeric_limits<float>::min();
        float m_smallestZ = std::numeric_limits<float>::max();
        float m_biggestZ = std::numeric_limits<float>::min();
        float m_mostBottomVertex = std::numeric_limits<float>::max();
        float m_mostTopVertex = std::numeric_limits<float>::min();

        float m_fromOriginToTop = 0.0f; // Distance between origin and character top. Before rotation (if has).
        float m_fromOriginToBottom = 0.0f; // Distance between origin and character bottom. Before rotation (if has).
        float m_XZRadius = 0.0f; // Radius on X/Z axis. From collision mesh origin. Before rotation (if has).
        float m_objectHeight = 0.0f;
        // Collider data end.

        // Physics data.
        PhysicsTransforms m_physicsTransforms;
        bool m_hasCollisionObject = false; // Set true for all collision objects.
        CollisionGroups m_collisionGroup = CollisionGroups::NONE; // Set inside colliding objects.
        CollisionGroups m_collisionMask = CollisionGroups::NONE; // Set inside colliding objects.
        CollisionFlags m_collisionFlag = CollisionFlags::NONE; // Set inside colliding objects.
        bool m_isEnabledInPhysicsSimulation = false; // Set inside colliding objects.
        float m_collisionMass = 0.0f;
        // Physics data end.

    private:
        // Only for internal checks inside this file.
        glm::vec3 m_linearFactor{1.0f, 1.0f, 1.0f};
        glm::vec3 m_angularFactor{1.0f, 1.0f, 1.0f};
        glm::vec3 m_gravity{std::numeric_limits<float>::max()};
        float m_linearDamping = -1.0f;
        float m_angularDamping = -1.0f;

        bool m_isDisabledForEver = false;
        bool m_isEnabledDraw = true; // For method draw().
        bool m_isEnabledUpdate = true; // For methods updateBeforePhysics() + updateAfterPhysics().

        // If objects on scene has face it should be created in Blender with face directed along +X axis.
        // Engine assume you create 3D models in Blender where up axis is +Z
        // and during exporting model you change axis to: up +Y, forward -Z.
        // That will add rotation to exported model: 90 degrees around -X axis or 270 degrees around +X axis.
        // If you want take models faceDir or upDir just multiply these vectors by m_rotateMatrix.
        const glm::vec4 m_sceneObjectFaceDir{1.0f, 0.0f, 0.0f, 1.0f};
        const glm::vec4 m_sceneObjectRightDir{0.0f, -1.0f, 0.0f, 1.0f};
        const glm::vec4 m_sceneObjectUpDir{0.0f, 0.0f, 1.0f, 1.0f};
    };
}
