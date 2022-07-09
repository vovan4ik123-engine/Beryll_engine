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
         * modelPath - path to models file (.DAE or .FBX). start path from first folder inside assets/
         * canBeDisabled - true if object can be disabled from game (update/draw/playsound/simulate physics)
         *                 object can be dissabled if camera dont see it or distance is too far. For performance
         * vertexPath - path to vertex shader for this model
         * fragmentPath - path to fragment shader for this model
         * diffSampler - name of sampler2D .... for diffuse texture in fragment shader
         *               sampler2D .... for diffuse texture MUST be first in shader
         * specSampler - name of sampler2D .... for specular texture in fragment shader
         *               sampler2D .... for specular texture MUST be second in shader
         */
        SimpleObject(const char* modelPath,  // common params
                     bool canBeDisabled,
                     const char* vertexPath, // graphics params
                     const char* fragmentPath,
                     const char* diffSampler,
                     const char* specSampler = nullptr);
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
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<Texture> m_diffTexture;
        const uint32_t m_diffSamplerIndexInShader = 0; // diffuse sampler MUST be first in fragment shader
        std::unique_ptr<Texture> m_specTexture;
        const uint32_t m_specSamplerIndexInShader = 1; // specular sampler MUST be second in fragment shader

        Assimp::Importer m_importer;
        const aiScene* m_scene = nullptr;
    };
}
