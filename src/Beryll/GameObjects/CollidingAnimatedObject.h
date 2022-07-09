#pragma once

#include "SceneObject.h"

namespace Beryll
{
    // Animated object, participates in physics simulation
    class CollidingAnimatedObject : public SceneObject
    {
    public:
        CollidingAnimatedObject() = delete;
        /*
         * modelPath - path to models file (.DAE or .FBX). start path from first folder inside assets/
         * canBeDisabled - true if object can be disabled from game (update/draw/playsound/simulate physics)
         *                 object can be dissabled if camera dont see it or distance is too far. For performance
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
        CollidingAnimatedObject(const char* modelPath,  // common params
                                bool canBeDisabled,
                                float collisionMass,    // physics params
                                bool wantCollisionCallBack,
                                CollisionFlags collFlag,
                                CollisionGroups collGroup,
                                CollisionGroups collMask,
                                const char* vertexPath, // graphics params
                                const char* fragmentPath,
                                const char* diffSampler,
                                const char* specSampler = nullptr);
        ~CollidingAnimatedObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

        void setAnimation(const char* name); // animations should be loaded from model

    protected:
        // animation data
        struct BoneMatrix // store loaded transforms for bone and final transform after frame interpolation
        {
            aiMatrix4x4 offsetMatrix{};
            aiMatrix4x4 finalWorldTransform{};
        };
        static constexpr uint32_t NUM_BONES_PER_VERTEX = 4; // one vertex can be affected maximum by 4 bones
        uint32_t m_boneCount = 0;
        std::map<std::string, uint32_t> m_boneNameIndex;
        std::vector<BoneMatrix> m_bonesMatrices;
        std::string m_boneMatrixNameInShader;
        std::map<std::string, uint32_t> m_animationNameIndex;
        uint32_t m_currentAnimIndex = 0;

        void calculateTransforms();
        void readNodeHierarchy(const float animationTime, const aiNode* node, const aiMatrix4x4& parentTransform);
        const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const aiString& nodeName);
        aiMatrix4x4 interpolatePosition(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor);
        aiMatrix4x4 interpolateRotation(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor);
        aiMatrix4x4 interpolateScaling(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor);

        aiMatrix4x4 m_globalInverseMatrix;
        // animation data end

        // model data
        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<VertexBuffer> m_boneIDsBuffer;
        std::shared_ptr<VertexBuffer> m_boneWeightsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<Texture> m_diffTexture;
        const uint32_t m_diffSamplerIndexInShader = 0; // diffuse sampler MUST be first in fragment shader
        std::unique_ptr<Texture> m_specTexture;
        const uint32_t m_specSamplerIndexInShader = 1; // specular sampler MUST be second in fragment shader
        // model data end

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