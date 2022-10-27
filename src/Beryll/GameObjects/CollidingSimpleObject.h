#pragma once

#include "BaseSimpleObject.h"
#include "Beryll/Renderer/Buffer.h"
#include "Beryll/Renderer/VertexArray.h"
#include "Beryll/Renderer/Shader.h"
#include "Beryll/Renderer/Texture.h"

namespace Beryll
{
    // Not animated object, participates in physics simulation
    class CollidingSimpleObject : public BaseSimpleObject
    {
    public:
        CollidingSimpleObject() = delete;
        /*
         * modelPath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * collisionMass - mass of this object for physics simulation. 0 for static objects
         *                 for example map ground should be alwaus enabled
         * wantCollisionCallBack - if true Physics module will store actual collisions for this object,
         *                         you can check it with Physics::getIsCollision(id1, id2) or Physics::getAllCollisions()
         * collFlag - type of collision object in physics world
         * collGroup - group or groups of current object in physics world
         * collMask - should contain collGroup or groups with which you want collisions
         */
        CollidingSimpleObject(const char* modelPath,  // common params
                              float collisionMass,    // physics params
                              bool wantCollisionCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask);
        ~CollidingSimpleObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

    protected:
        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_vertexTangentsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader. Use if no shader was bound on scene
        std::unique_ptr<Texture> m_diffTexture;
        std::unique_ptr<Texture> m_specTexture;

        // collision mesh dimensions
        float m_smallestX = std::numeric_limits<float>::max();
        float m_biggestX = std::numeric_limits<float>::min();
        float m_smallestZ = std::numeric_limits<float>::max();
        float m_biggestZ = std::numeric_limits<float>::min();
        float m_mostBottomVertex = std::numeric_limits<float>::max();
        float m_mostTopVertex = std::numeric_limits<float>::min();

    private:
        void processCollisionMesh(const aiScene* scene,
                                  const aiMesh* mesh,
                                  const std::string& meshName,
                                  float mass,
                                  bool wantCallBack,
                                  CollisionFlags collFlag,
                                  CollisionGroups collGroup,
                                  CollisionGroups collMask);
    };
}
