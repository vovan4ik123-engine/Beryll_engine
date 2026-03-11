#include "AndroidGLESGUIText.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include "beryll/utils/File.h"
#include "beryll/renderer/Camera.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/platform/androidGLES/AndroidGLESGlobal.h"

namespace Beryll
{
    bool AndroidGLESGUIText::m_initialized = false;
    std::map<char, AndroidGLESGUIText::Character> AndroidGLESGUIText::m_characters;

    AndroidGLESGUIText::AndroidGLESGUIText(std::string pText,
                                           const glm::vec3& color, const glm::vec3& pos, const float height)
                                           : GUIText(pos, glm::vec2{1.0f})
    {
        text = std::move(pText);
        m_color = color;
        m_currentHeightPixels = Window::getInstance()->getScreenHeight() * 0.2f;
        m_desiredHeightPixels = (height / 100.0f) * Window::getInstance()->getScreenHeight();
        m_scale = m_desiredHeightPixels / m_currentHeightPixels;

        if(!m_initialized)
        {
            m_initialized = true;
            BR_INFO("%s", "Initialize AndroidGLESGUIText.");

            // Init library.
            FT_Library ftLib;
            if(FT_Init_FreeType(&ftLib) != 0)
            {
                BR_ASSERT(false, "%s", "ERROR FREETYPE can not init FreeType Library.");
            }

            // Load font into face.
            FT_Face ftFace;
            uint32_t fontBufferSize = 0;
            char* fontBuffer = BeryllUtils::File::readToBuffer("fonts/roboto.ttf", &fontBufferSize);
            if(FT_New_Memory_Face(ftLib, reinterpret_cast<unsigned char*>(fontBuffer), fontBufferSize, 0, &ftFace)  != 0)
            {
                BR_ASSERT(false, "%s", "ERROR FREETYPE can not load font: fonts/roboto.ttf.");
            }

            FT_Set_Pixel_Sizes(ftFace, 0, FT_UInt(m_currentHeightPixels));

            // Disable byte-alignment restriction.
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // Create OpenGL textures for each glyph from ASCII table.
            for (unsigned char c = 32; c < 128; c++)
            {
                // Load character glyph.
                if(FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
                {
                    BR_WARN("Freetype can not load char %s", c);
                    continue;
                }

                // Generate texture.
                unsigned int textureID;
                glGenTextures(1, &textureID);
                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             GL_RED,
                             ftFace->glyph->bitmap.width,
                             ftFace->glyph->bitmap.rows,
                             0,
                             GL_RED,
                             GL_UNSIGNED_BYTE,
                             ftFace->glyph->bitmap.buffer
                );
                // Set texture options.
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);

                // Store character for later use.
                Character character
                        {
                                textureID,
                                glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows),
                                glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top),
                                ftFace->glyph->advance.x >> 6, // Bit shift by 6 to get value in pixels. Same as x / 64.
                        };

                m_characters.insert(std::pair<char, Character>(c, character));
            }

            FT_Done_Face(ftFace);
            FT_Done_FreeType(ftLib);
            delete[] fontBuffer;

            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }

        m_internalShader = Renderer::createShader(BeryllConstants::GUITextVertexPath.data(),
                                                  BeryllConstants::GUITextFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();
    }

    AndroidGLESGUIText::~AndroidGLESGUIText()
    {

    }

    void AndroidGLESGUIText::updateBeforePhysics()
    {

    }

    void AndroidGLESGUIText::updateAfterPhysics()
    {

    }

    void AndroidGLESGUIText::draw()
    {
        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());
        m_internalShader->set3Float("textColor", m_color);
        glActiveTexture(GL_TEXTURE0);

        glm::vec3 currentCarriagePos = getPositionInPixels();

        m_vertices[0].z = currentCarriagePos.z; // Z will not changing for all characters. X, Y will calculated in loop.
        m_vertices[1].z = currentCarriagePos.z;
        m_vertices[2].z = currentCarriagePos.z;
        m_vertices[3].z = currentCarriagePos.z;

        // Iterate through all characters.
        for (char ch : text)
        {
            auto chFromMap= m_characters.find(ch);
            if(chFromMap == m_characters.end())
            {
                BR_ASSERT(false, "Cant find char %s", ch);
            }

            // In pixels (screen resolution).
            float xPos = currentCarriagePos.x + chFromMap->second.bearing.x * m_scale;
            float yPos = currentCarriagePos.y - (chFromMap->second.size.y - chFromMap->second.bearing.y) * m_scale;
            // Move from screen resolution range to -1...1 (prepare for camera projection matrix).
            xPos = (xPos / Window::getInstance()->getScreenWidth()) * 2.0f - 1.0f;
            yPos = (yPos / Window::getInstance()->getScreenHeight()) * 2.0f - 1.0f;

            float w = chFromMap->second.size.x * m_scale;
            float h = chFromMap->second.size.y * m_scale;
            w = (w / Window::getInstance()->getScreenWidth()) * 2.0f;
            h = (h / Window::getInstance()->getScreenHeight()) * 2.0f;

            m_vertices[0].x = xPos;      m_vertices[0].y = yPos;
            m_vertices[1].x = xPos + w;  m_vertices[1].y = yPos;
            m_vertices[2].x = xPos + w;  m_vertices[2].y = yPos + h;
            m_vertices[3].x = xPos;      m_vertices[3].y = yPos + h;

            m_vertexPosBuffer->setDynamicBufferData(m_vertices, m_vertices.size());

            currentCarriagePos.x += (chFromMap->second.advance * m_scale);

            glBindTexture(GL_TEXTURE_2D, chFromMap->second.textureID);

            m_vertexArray->bind();
            m_vertexArray->draw();
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        GLESStateVariables::currentDiffuseTextureMat1ID0 = 0;
    }
}
