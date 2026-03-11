#pragma once

#include "beryll/GUI/GUIText.h"

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
         * height - height of text in screen percents (0...100).
         */
        AndroidGLESGUIText(std::string pText,
                           const glm::vec3& color, const glm::vec3& pos, const float height);

        struct Character
        {
            unsigned int textureID; // ID handle of the glyph texture.
            glm::ivec2   size;      // Size of glyph.
            glm::ivec2   bearing;   // Offset from baseline to left/top of glyph.
            long advance;           // Offset to advance to next glyph.
        };

        float m_currentHeightPixels = 0.0f;
        float m_desiredHeightPixels = 0.0f;
        float m_scale = 1.0f;

        static bool m_initialized;
        static std::map<char, Character> m_characters;

        // Vertex and index buffers are in base class.
        // ........
    };
}
