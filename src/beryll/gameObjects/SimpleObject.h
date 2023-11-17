#pragma once

#include "BaseSimpleObject.h"
#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"
#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    // Not animated object, not participates in physics simulation.
    class SimpleObject : public BaseSimpleObject
    {
    public:
        SimpleObject() = delete;
        /*
         * filePath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * objGroup - game specific group to which this scene object belong
         */
        SimpleObject(const char* filePath,
                     SceneObjectGroups sceneGroup);
        SimpleObject(const std::string& filePath,
                     const aiScene* scene,
                     const aiMesh* graphicsMesh,
                     SceneObjectGroups sceneGroup);
        ~SimpleObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        // All loaded objects will have same sceneGroup.
        static std::vector<std::shared_ptr<SimpleObject>> loadManyModelsFromOneFile(const char* filePath, SceneObjectGroups sceneGroup);

    protected:
        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_vertexTangentsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader.
        std::unique_ptr<Texture> m_diffTexture;
        std::unique_ptr<Texture> m_specTexture;
        std::unique_ptr<Texture> m_normalMapTexture;

    private:
        void loadGraphicsMesh(const std::string& filePath, const aiScene* scene, const aiMesh* graphicsMesh);
    };
}
