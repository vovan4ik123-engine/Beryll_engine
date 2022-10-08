#pragma once

#include "SceneObject.h"

namespace Beryll
{
    // Animated object, not participates in physics simulation
    class AnimatedObject : public SceneObject
    {
    public:
        AnimatedObject() = delete;
        /*
         * modelPath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         */
        AnimatedObject(const char* modelPath);
        ~AnimatedObject() override;

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
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader. Use if no shader was bound on scene
        std::unique_ptr<Texture> m_diffTexture;
        std::unique_ptr<Texture> m_specTexture;
        // model data end

        const aiScene* m_scene = nullptr;

    private:
        // aiScene + Assimp::Importer lifetime for animated object should be same as animated object
        // aiScene + Assimp::Importer has big size in memory
        // this map will share same scene across animated objects loaded from same file
        // id = file path. If other object load model from same file(id = file path) it will get pointer to aiScene from this map
        static std::map<const std::string, std::pair<std::shared_ptr<Assimp::Importer>, const aiScene*>> m_importersScenes;

        const std::string m_modelPath; // object ID in m_importersScenes map
    };
}
