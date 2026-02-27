#include "GUIObject.h"
#include "beryll/renderer/Renderer.h"

namespace Beryll
{
    void GUIObject::setBuffers()
    {
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

        updateBuffersWithPositions(); // Only after buffers created.
    }

    void GUIObject::updateBuffersWithPositions()
    {
        if(m_vertexPosBuffer == nullptr)
        {
            BR_ASSERT(false, "%s", "Buffer is nullptr.");
            return;
        }
        // Move coords to GUI screenSpace -1...1.
        glm::vec3 screenSpacePos = getPositionNormalized();
        screenSpacePos.x = screenSpacePos.x * 2.0f - 1.0f;
        screenSpacePos.y = screenSpacePos.y * 2.0f - 1.0f;
        glm::vec2 WH = getWidthHeightNormalized() * 2.0f;
        std::vector<glm::vec3> vertices{glm::vec3(screenSpacePos.x,         screenSpacePos.y,        screenSpacePos.z),
                                        glm::vec3(screenSpacePos.x + WH.x,  screenSpacePos.y,        screenSpacePos.z),
                                        glm::vec3(screenSpacePos.x + WH.x,  screenSpacePos.y + WH.y, screenSpacePos.z),
                                        glm::vec3(screenSpacePos.x,         screenSpacePos.y + WH.y, screenSpacePos.z)};

        m_vertexPosBuffer->setDynamicBufferData(vertices, vertices.size());
    }
}
