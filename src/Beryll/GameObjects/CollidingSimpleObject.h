#pragma once

#include "SceneObject.h"

namespace Beryll
{
    // Not animated object, participates in physics simulation
    class CollidingSimpleObject : public SceneObject
    {
    public:
        CollidingSimpleObject() = delete;
        /*
         * modelPath - path to models file (.DAE or .FBX). start path from first folder inside assets/
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
        CollidingSimpleObject(const char* modelPath,  // common params
                              float collisionMass,    // physics params
                              bool wantCollisionCallBack,
                              CollisionFlags collFlag,
                              CollisionGroups collGroup,
                              CollisionGroups collMask,
                              const char* vertexPath, // graphics params
                              const char* fragmentPath,
                              const char* diffSampler,
                              const char* specSampler = nullptr);
        ~CollidingSimpleObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

    protected:
        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<Texture> m_diffTexture;
        const uint32_t m_diffSamplerIndexInShader = 0; // diffuse sampler MUST be first in fragment shader
        std::unique_ptr<Texture> m_specTexture;
        const uint32_t m_specSamplerIndexInShader = 1; // specular sampler MUST be second in fragment shader

        Assimp::Importer m_importer;
        const aiScene* m_scene = nullptr;

    private:
        void processCollisionMesh(const aiMesh* mesh,
                                  const std::string& meshName,
                                  float mass,
                                  bool wantCallBack,
                                  CollisionFlags collFlag,
                                  CollisionGroups collGroup,
                                  CollisionGroups collMask);
    };
}
