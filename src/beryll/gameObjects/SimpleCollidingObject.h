#pragma once

#include "BaseSimpleObject.h"
#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"
#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Material.h"

namespace Beryll
{
    // Not animated object, participates in physics simulation.
    class SimpleCollidingObject : public BaseSimpleObject
    {
    public:
        SimpleCollidingObject() = delete;
        /*
         * filePath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * collisionMassKg - mass of this object for physics simulation. 0 for static objects
         * wantCollisionCallBack - drop performance too much because call back use std::scoped_lock<std::mutex>
         *                         if true Physics module will store actual collisions for this object,
         *                         you can check it with Physics::getIsCollision(id1, id2) or Physics::getAllCollisions()
         * collFlag - type of collision object in physics world
         * collGroup - group or groups of current object in physics world
         * collMask - should contain collGroup or groups with which you want collisions
         * sceneGroup - game specific group to which this scene object belong
         */
        SimpleCollidingObject(const char* filePath,  // Common params.
                              float collisionMassKg,    // Physics params.
                              bool wantCollisionCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask,
                              SceneObjectGroups sceneGroup);
        SimpleCollidingObject(const std::string& filePath,
                              const aiScene* scene,
                              const aiMesh* graphicsMesh,
                              const aiMesh* collisionMesh,
                              const std::string& collisionMeshName,
                              float collisionMassKg,
                              bool wantCollisionCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask,
                              SceneObjectGroups sceneGroup);
        ~SimpleCollidingObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void addMaterial2(const std::string& diffusePath,
                          const std::string& specularPath,
                          const std::string& normalMapPath,
                          const std::string& blendTexturePath) override;

        // All loaded objects will have same parameters(mass, flags, groups, ...).
        static std::vector<std::shared_ptr<SimpleCollidingObject>> loadManyModelsFromOneFile(const char* filePath,
                                                                                             float collisionMassKg,
                                                                                             bool wantCollisionCallBack,
                                                                                             CollisionFlags collFlag,
                                                                                             CollisionGroups collGroup,
                                                                                             CollisionGroups collMask,
                                                                                             SceneObjectGroups sceneGroup);

    protected:
        // Collision mesh dimensions.
        float m_smallestX = std::numeric_limits<float>::max();
        float m_biggestX = std::numeric_limits<float>::min();
        float m_smallestZ = std::numeric_limits<float>::max();
        float m_biggestZ = std::numeric_limits<float>::min();
        float m_mostBottomVertex = std::numeric_limits<float>::max();
        float m_mostTopVertex = std::numeric_limits<float>::min();

    private:
        void loadGraphicsMesh(const std::string& filePath, const aiScene* scene, const aiMesh* graphicsMesh);

        void loadCollisionMesh(const aiScene* scene,
                               const aiMesh* collisionMesh,
                               const std::string& meshName,
                               float mass,
                               bool wantCallBack,
                               CollisionFlags collFlag,
                               CollisionGroups collGroup,
                               CollisionGroups collMask);

        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_vertexTangentsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader.
        Material1 m_material1;
        std::optional<Material2> m_material2;
        // Can be used in shader to return UV coords in range 0...1 if was scaled. Useful if we have m_material2 with blend texture.
        // Shader code example: vec2 blendTextureUV = (inUV + m_addToUVCoords) * m_UVCoordsMultiplier;
        float m_addToUVCoords = 0.0f;
        float m_UVCoordsMultiplier = 0.0f;
    };
}
