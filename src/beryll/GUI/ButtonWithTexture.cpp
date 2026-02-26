#include "ButtonWithTexture.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    ButtonWithTexture::ButtonWithTexture(const char* defaultTexturePath,
                                         const char* touchedTexturePath,
                                         const glm::vec3& pos, const glm::vec2& widthHeight, bool actRepeat)
    {
        BR_ASSERT((defaultTexturePath != nullptr && defaultTexturePath[0] != '\0'), "%s", "Path to default texture can not be empty.");

        setPositionInPercents(pos);
        setWidthHeightInPercents(widthHeight);
        m_actRepeat = actRepeat;

        m_defaultTexture = Renderer::createTexture(defaultTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

        if(touchedTexturePath != nullptr && touchedTexturePath[0] != '\0')
            m_touchedTexture = Renderer::createTexture(touchedTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

#if defined(ANDROID)
        // Vertices created as dynamic buffer. Will be updated in updateBuffersWithPositions().
        std::vector<glm::vec3> vertices{glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f)};

        std::vector<glm::vec2> textureCoords{glm::vec2(0.0f, 1.0f), // Flipped Y for OpenGL.
                                             glm::vec2(1.0f, 1.0f),
                                             glm::vec2(1.0f, 0.0f),
                                             glm::vec2(0.0f, 0.0f)};

        std::vector<uint32_t> indices{0,1,2,
                                      2,3,0};
#elif defined(APPLE)

#endif

        m_vertexPosBuffer = Renderer::createDynamicVertexBuffer(VertexAttribType::FLOAT, VertexAttribSize::THREE, sizeof(glm::vec3) * vertices.size());
        m_textureCoordsBuffer = Renderer::createStaticVertexBuffer(textureCoords);
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader(BeryllConstants::GUIElementWithTextureVertexPath.data(),
                                                  BeryllConstants::GUIElementWithTextureFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();

        updateBuffersWithPositions(); // Only after buffers created.
    }

    ButtonWithTexture::~ButtonWithTexture()
    {

    }

    void ButtonWithTexture::updateBeforePhysics()
    {
        std::vector<Finger>& fingers = EventHandler::getFingers();

        if(fingers.empty())
        {
            m_pressed = false;
            m_touched = false;
            m_pressedFingerID = -100;
            m_isPressedFingerStillOnScreen = false;
        }
        else
        {
            if(m_actRepeat && m_pressed)
            {
                m_pressed = false;
                for(const Finger& f : fingers)
                {
                    // Flipper Y for opengl
                    glm::vec2 flippedY = f.normalizedPos;
                    flippedY.y = 1.0f - flippedY.y;

                    if(flippedY.x > getPositionNormalized().x && flippedY.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
                       flippedY.y > getPositionNormalized().y && flippedY.y < getPositionNormalized().y + getWidthHeightNormalized().y)
                    {
                        // If any finger in button area.
                        m_pressed = true;
                    }
                }
            }
            else
            {
                m_pressed = false;
            }

            m_touched = false;
            for(Finger& f : fingers)
            {
                // Flipper Y for opengl
                glm::vec2 flippedY = f.normalizedPos;
                flippedY.y = 1.0f - flippedY.y;

                if(flippedY.x > getPositionNormalized().x && flippedY.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
                   flippedY.y > getPositionNormalized().y && flippedY.y < getPositionNormalized().y + getWidthHeightNormalized().y)
                {
                    if(f.ID == m_pressedFingerID)
                        m_touched = true;

                    if(f.downEvent && !f.handled)
                    {
                        f.handled = true;
                        m_pressed = true;
                        m_pressedFingerID = f.ID;
                        m_isPressedFingerStillOnScreen = true;
                    }
                }
            }

            m_isPressedFingerStillOnScreen = false;
            for(const Finger& f : fingers)
            {
                if(f.ID == m_pressedFingerID)
                    m_isPressedFingerStillOnScreen = true;
            }

            if(!m_isPressedFingerStillOnScreen)
                m_pressedFingerID = -100;
        }

        if(m_pressed && m_action)
        {
            m_action();
        }
    }

    void ButtonWithTexture::updateAfterPhysics()
    {

    }

    void ButtonWithTexture::draw()
    {
        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());

        if(m_touched && m_touchedTexture)
            m_touchedTexture->bind();
        else
            m_defaultTexture->bind();

        m_vertexArray->bind();
        m_vertexArray->draw();
    }
}
