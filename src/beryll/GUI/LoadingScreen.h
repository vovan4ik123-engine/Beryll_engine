#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/RandomGenerator.h"
#include "beryll/core/Log.h"
#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"
#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class LoadingScreen
    {
    public:
        LoadingScreen() = delete;
        ~LoadingScreen() = delete;

        static void show();

        static void selectRandomTexture()
        {
            BR_ASSERT((!m_diffuseTextures.empty()), "%s", "Loading screen textures are empty. Use setTextures().");

            m_textureIndex = Beryll::RandomGenerator::getInt(m_diffuseTextures.size() - 1);
        }

        static void setTextureByIndex(int index)
        {
            BR_ASSERT((!m_diffuseTextures.empty()), "%s", "Loading screen textures are empty. Use setTextures().");

            if(index >= 0 && index < m_diffuseTextures.size())
                m_textureIndex = index;
            else
                m_textureIndex = Beryll::RandomGenerator::getInt(m_diffuseTextures.size() - 1);
        }

        static void setTextures(const std::vector<std::string>& screenImages);

    private:
        friend class GameLoop;
        static void create();
        static bool m_created;

        static std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        static std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        static std::shared_ptr<IndexBuffer> m_indexBuffer;
        static std::unique_ptr<VertexArray> m_vertexArray;
        static std::shared_ptr<Shader> m_internalShader;
        static std::vector<std::unique_ptr<Texture>> m_diffuseTextures;

        static int m_textureIndex;
    };
}
