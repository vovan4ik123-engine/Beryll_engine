#pragma once

#include "BaseSimpleObject.h"
#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"

namespace Beryll
{
    // Not animated object, not participates in physics simulation.
    class SimpleObject : public BaseSimpleObject
    {
    public:
        SimpleObject() = delete;
        /*
         * filePath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * sceneGroup - game specific group to which this scene object belong
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

        void addMaterial2(const std::string& diffusePath,
                          const std::string& specularPath,
                          const std::string& normalMapPath,
                          const std::string& blendTexturePath) override;

        // All loaded objects will have same sceneGroup.
        static std::vector<std::shared_ptr<SimpleObject>> loadManyModelsFromOneFile(const char* filePath, SceneObjectGroups sceneGroup);

    private:
        void loadGraphicsMesh(const std::string& filePath, const aiScene* scene, const aiMesh* graphicsMesh);

        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_vertexTangentsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
    };
}
