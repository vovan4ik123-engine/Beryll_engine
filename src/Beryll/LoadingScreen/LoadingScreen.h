#pragma once

#include "CppHeaders.h"
#include "LibsHeaders.h"

#include "Beryll/Core/GameLoop.h"
#include "Beryll/Renderer/Shader.h"
#include "Beryll/Renderer/Buffer.h"
#include "Beryll/Renderer/VertexArray.h"
#include "Beryll/Renderer/Texture.h"
#include "Beryll/Core/RandomGenerator.h"

namespace Beryll
{
    class LoadingScreen
    {
    public:
        LoadingScreen() = delete;
        ~LoadingScreen() = delete;

        static void showProgress(float pr);

        static float getProgress()
        {
            return m_loadingProgress * 100.0f;
        }

        static void selectRandomTexture()
        {
            BR_ASSERT((!m_diffuseTextures.empty()), "%s", "Loading screen textures are empty. Use setTextures()");

            m_textureIndex = Beryll::RandomGenerator::getInt(m_diffuseTextures.size() - 1);
        }

        static void setTextures(const std::vector<std::string>& screenImages);

    private:
        friend class GameLoop;
        static void create();

        static std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        static std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        static std::shared_ptr<IndexBuffer> m_indexBuffer;
        static std::unique_ptr<VertexArray> m_vertexArray;
        static std::shared_ptr<Shader> m_internalShader;
        static std::vector<std::unique_ptr<Texture>> m_diffuseTextures;

        static float m_loadingProgress; // 0.0f...1.0f range
        static int m_textureIndex;
        static bool m_created;
    };
}
