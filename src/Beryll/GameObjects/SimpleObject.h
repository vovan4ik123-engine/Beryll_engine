#pragma once

#include "BaseSimpleObject.h"
#include "Beryll/Renderer/Buffer.h"
#include "Beryll/Renderer/VertexArray.h"
#include "Beryll/Renderer/Shader.h"
#include "Beryll/Renderer/Texture.h"

namespace Beryll
{
    // Not animated object, not participates in physics simulation
    class SimpleObject : public BaseSimpleObject
    {
    public:
        SimpleObject() = delete;
        /*
         * modelPath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         */
        SimpleObject(const char* modelPath);
        ~SimpleObject() override;

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
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader. Use if no shader was bound on scene
        std::unique_ptr<Texture> m_diffTexture;
        std::unique_ptr<Texture> m_specTexture;
    };
}
