#include "CheckBox.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    CheckBox::CheckBox(const char* unMarkedTexturePath,
                       const char* markedTexturePath,
                       const glm::vec3& pos, const glm::vec2& widthHeight)
    {
        BR_ASSERT((unMarkedTexturePath != nullptr && unMarkedTexturePath[0] != '\0'), "%s", "Path to unMarked Texture can not be empty.");
        BR_ASSERT((markedTexturePath != nullptr && markedTexturePath[0] != '\0'), "%s", "Path to marked Texture can not be empty.");

        setPositionInPercents(pos);
        setWidthHeightInPercents(widthHeight);

        m_unMarkedTexture = Renderer::createTexture(unMarkedTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_markedTexture = Renderer::createTexture(markedTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

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

    CheckBox::~CheckBox()
    {

    }

    void CheckBox::updateBeforePhysics()
    {
        m_marking = false;
        m_unMarking = false;

        std::vector<Finger>& fingers = EventHandler::getFingers();
        for(Finger& f : fingers)
        {
            // Flipper Y for opengl
            glm::vec2 flippedY = f.normalizedPos;
            flippedY.y = 1.0f - flippedY.y;

            if(flippedY.x > getPositionNormalized().x && flippedY.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
               flippedY.y > getPositionNormalized().y && flippedY.y < getPositionNormalized().y + getWidthHeightNormalized().y)
            {
                // If any finger in checkbox area.
                if(f.downEvent && !f.handled)
                {
                    f.handled = true;

                    if(marked)
                    {
                        marked = false;
                        m_marking = false;
                        m_unMarking = true;
                    }
                    else
                    {
                        marked = true;
                        m_marking = true;
                        m_unMarking = false;
                    }
                }
            }
        }

        if(m_marking && m_action)
        {
            m_action();
        }
    }

    void CheckBox::updateAfterPhysics()
    {

    }

    void CheckBox::draw()
    {
        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());

        if(marked)
            m_markedTexture->bind();
        else
            m_unMarkedTexture->bind();

        m_vertexArray->bind();
        m_vertexArray->draw();
    }
}
