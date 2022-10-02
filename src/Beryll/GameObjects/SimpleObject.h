#pragma once

#include "SceneObject.h"

namespace Beryll
{
    // Not animated object, not participates in physics simulation
    class SimpleObject : public SceneObject
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

        void setShader(std::shared_ptr<Shader> shader)
        {
            m_shader = std::move(shader);
        }

    protected:
        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_vertexNormalsBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_shader;
        std::unique_ptr<Texture> m_diffTexture;
        std::unique_ptr<Texture> m_specTexture;
    };
}
