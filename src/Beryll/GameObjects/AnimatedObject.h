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
        void setShader(std::shared_ptr<Shader> shader)
        {
            m_shader = std::move(shader);
        }

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
        std::shared_ptr<Shader> m_shader;
        std::unique_ptr<Texture> m_diffTexture;
        std::unique_ptr<Texture> m_specTexture;
        // model data end

        Assimp::Importer m_importer;
        const aiScene* m_scene = nullptr;
    };
}
