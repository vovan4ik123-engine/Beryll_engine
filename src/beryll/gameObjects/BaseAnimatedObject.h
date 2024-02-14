#pragma once

#include "SceneObject.h"

namespace Beryll
{
    class BaseAnimatedObject : public SceneObject
    {
    protected:
        struct BoneMatrix // store loaded transforms for bone and final transform after frame interpolation
        {
            aiMatrix4x4 offsetMatrix{};
            aiMatrix4x4 finalWorldTransform{};
        };

    public:
        BaseAnimatedObject() = delete;
        ~BaseAnimatedObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        uint32_t getBoneCount() { return m_boneCount; }
        const std::vector<BoneMatrix>& getBoneMatrices() { return m_bonesMatrices; }
        bool getIsOneTimeAnimationFinished() { return !m_playAnimOneTime; }

        void setCurrentAnimationByName(const char* name, bool playOneTime, bool startEvenIfSameAnimPlaying);
        void setCurrentAnimationByIndex(int index, bool playOneTime, bool startEvenIfSameAnimPlaying);
        void setDefaultAnimationByName(const char* name);
        void setDefaultAnimationByIndex(int index);

        // Call it sometimes between game levels/maps to free some memory.
        // Or dont call if you will load same models again. They will be taken from cache for faster loading.
        static void clearCachedModels() { m_importersScenes.clear(); };

    protected:
        BaseAnimatedObject(const char* filePath,
                           SceneObjectGroups sceneGroup);

        // aiScene + Assimp::Importer lifetime for animated object should be same as animated object.
        // aiScene + Assimp::Importer has big size in memory.
        // This map will cache animated objects and reuse them if you load again from same file.
        // If many of AnimatedCollidingObject load model from same file(id = file path) it will get pointer to aiScene from this map after first loading.
        // id = file path.
        static std::map<const std::string, std::pair<std::shared_ptr<Assimp::Importer>, const aiScene*>> m_importersScenes;

        const std::string m_modelPath; // Object ID in m_importersScenes map.

        // Animation data.
        static constexpr uint32_t NUM_BONES_PER_VERTEX = 4; // One vertex can be affected maximum by 4 bones.
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

        const aiScene* m_scene = nullptr;
    };
}
