#pragma once

#include "BaseAnimatedObject.h"
#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"
#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    // Animated object, participates in physics simulation
    class AnimatedCollidingObject : public BaseAnimatedObject
    {
    public:
        AnimatedCollidingObject() = delete;
        /*
         * modelPath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * collisionMass - mass of this object for physics simulation. 0 for static objects
         * wantCollisionCallBack - drop performance too much because call back use std::scoped_lock<std::mutex>
         *                         if true Physics module will store actual collisions for this object,
         *                         you can check it with Physics::getIsCollision(id1, id2) or Physics::getAllCollisions()
         * collFlag - type of collision object in physics world
         * collGroup - group or groups of current object in physics world
         * collMask - should contain collGroup or groups with which you want collisions
         * objGroup - game specific group to which this scene object belong
         */
        AnimatedCollidingObject(const char* modelPath,  // common params
                                float collisionMass,    // physics params
                                bool wantCollisionCallBack,
                                CollisionFlags collFlag,
                                CollisionGroups collGroup,
                                CollisionGroups collMask,
                                SceneObjectGroups sceneGroup = SceneObjectGroups::NONE);
        ~AnimatedCollidingObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void setCurrentAnimationByName(const char* name, bool playOneTime) override; // animations should be loaded from model
        void setCurrentAnimationByIndex(int index, bool playOneTime) override;
        void setDefaultAnimationByName(const char* name) override;
        void setDefaultAnimationByIndex(int index) override;

        uint32_t getBoneCount() override
        {
            return m_boneCount;
        }

        const std::vector<BoneMatrix>& getBoneMatrices() override
        {
            return m_bonesMatrices;
        }

    protected:
        // animation data
        static constexpr uint32_t NUM_BONES_PER_VERTEX = 4; // one vertex can be affected maximum by 4 bones
        uint32_t m_boneCount = 0;
        std::vector<std::pair<std::string, uint32_t>> m_boneNameIndex;
        std::vector<BoneMatrix> m_bonesMatrices;
        std::string m_boneMatrixNameInShader;
        std::vector<std::pair<std::string, int>> m_animationNameIndex;
        int m_currentAnimIndex = 0;
        int m_defaultAnimIndex = 0;
        std::string m_currentAnimName;
        float m_animStartTimeInSec = 0.0f;
        float m_animTimeInSec = 0.0f;
        bool m_playAnimOneTime = false;

        void calculateTransforms();
        void readNodeHierarchy(const float animationTime, const aiNode* node, const aiMatrix4x4& parentTransform);
        const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const aiString& nodeName);
        aiMatrix4x4 interpolatePosition(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor);
        aiMatrix4x4 interpolateRotation(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor);
        aiMatrix4x4 interpolateScaling(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor);

        aiMatrix4x4 m_globalInverseMatrix;
        // animation data end

        // model data
        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_vertexTangentsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<VertexBuffer> m_boneIDsBuffer;
        std::shared_ptr<VertexBuffer> m_boneWeightsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader. Use if no shader was bound on scene
        std::unique_ptr<Texture> m_diffTexture;
        std::unique_ptr<Texture> m_specTexture;
        std::unique_ptr<Texture> m_normalMapTexture;
        // model data end

        const aiScene* m_scene = nullptr;

        // collision mesh dimensions
        float m_smallestX = std::numeric_limits<float>::max();
        float m_biggestX = std::numeric_limits<float>::min();
        float m_smallestZ = std::numeric_limits<float>::max();
        float m_biggestZ = std::numeric_limits<float>::min();
        float m_mostBottomVertex = std::numeric_limits<float>::max();
        float m_mostTopVertex = std::numeric_limits<float>::min();

    private:
        void processCollisionMesh(const aiMesh* mesh,
                                  const std::string& meshName,
                                  float mass,
                                  bool wantCallBack,
                                  CollisionFlags collFlag,
                                  CollisionGroups collGroup,
                                  CollisionGroups collMask);

        // aiScene + Assimp::Importer lifetime for animated object should be same as animated object
        // aiScene + Assimp::Importer has big size in memory
        // this map will share same scene across animated objects loaded from same file
        // id = file path. If other object load model from same file(id = file path) it will get pointer to aiScene from this map
        static std::map<const std::string, std::pair<std::shared_ptr<Assimp::Importer>, const aiScene*>> m_importersScenes;

        const std::string m_modelPath; // object ID in m_importersScenes map
    };
}