#pragma once

#include "BaseAnimatedObject.h"
#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"
#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    // Animated object, not participates in physics simulation.
    class AnimatedObject : public BaseAnimatedObject
    {
    public:
        AnimatedObject() = delete;
        /*
         * filePath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * sceneGroup - game specific group to which this scene object belong
         */
        AnimatedObject(const char* filePath,
                       SceneObjectGroups sceneGroup);
        ~AnimatedObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void setCurrentAnimationByName(const char* name, bool playOneTime, bool startEvenIfSameAnimPlaying) override;
        void setCurrentAnimationByIndex(int index, bool playOneTime, bool startEvenIfSameAnimPlaying) override;
        void setDefaultAnimationByName(const char* name) override;
        void setDefaultAnimationByIndex(int index) override;
        bool getIsOneTimeAnimationFinished() override { return !m_playAnimOneTime; }

        uint32_t getBoneCount() override
        {
            return m_boneCount;
        }

        const std::vector<BoneMatrix>& getBoneMatrices() override
        {
            return m_bonesMatrices;
        }

        // Call it sometimes between game levels/maps to free some memory.
        // Or dont call if you will load same models again. They will be taken from cache for faster loading.
        static void clearCachedModels() { m_importersScenes.clear(); };

    protected:
        // Animation data.
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
        // Animation data end.

        // Model data.
        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_vertexTangentsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<VertexBuffer> m_boneIDsBuffer;
        std::shared_ptr<VertexBuffer> m_boneWeightsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader.
        std::unique_ptr<Texture> m_diffTexture;
        std::unique_ptr<Texture> m_specTexture;
        std::unique_ptr<Texture> m_normalMapTexture;
        // Model data end.

        const aiScene* m_scene = nullptr;

    private:
        // aiScene + Assimp::Importer lifetime for animated object should be same as animated object.
        // aiScene + Assimp::Importer has big size in memory.
        // This map will cache animated objects and reuse them if you load again from same file.
        // If many of AnimatedCollidingObject load model from same file(id = file path) it will get pointer to aiScene from this map after first loading.
        // id = file path.
        static std::map<const std::string, std::pair<std::shared_ptr<Assimp::Importer>, const aiScene*>> m_importersScenes;

        const std::string m_modelPath; // object ID in m_importersScenes map
    };
}
