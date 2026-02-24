#pragma once

#include "beryll/GUI/GUIText.h"
#include "beryll/renderer/Renderer.h"

namespace Beryll
{
    class AndroidGLESGUIText : public GUIText
    {
    public:
        AndroidGLESGUIText() = delete;
        ~AndroidGLESGUIText() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        friend class Renderer;
        /*
         * pText - text to render.
         * color - color of text.
         * pos - X,Y in screen percents (0...100), Z in value as is (0...1).
         * scale - texts scale.
         */
        AndroidGLESGUIText(std::string pText, const glm::vec3& color, const glm::vec3& pos, const float scale);

        struct Character
        {
            unsigned int textureID; // ID handle of the glyph texture.
            glm::ivec2   size;      // Size of glyph.
            glm::ivec2   bearing;   // Offset from baseline to left/top of glyph.
            long advance;           // Offset to advance to next glyph.
        };

        static bool m_initialized;
        static std::map<char, Character> m_characters;

        // Vertices created as dynamic buffer. Will be updated during draw.
        std::vector<glm::vec3> m_vertices{glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec3(0.0f, 0.0f, 0.0f)};

        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader;
    };
}
