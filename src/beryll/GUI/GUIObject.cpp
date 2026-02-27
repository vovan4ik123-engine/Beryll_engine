#include "GUIObject.h"
#include "beryll/renderer/Renderer.h"

namespace Beryll
{
    GUIObject::GUIObject(const glm::vec3& pos, const glm::vec2& widthHeight)
    {
        setPositionInPercents(pos);
        setWidthHeightInPercents(widthHeight);

        // Create, then update buffers.
        m_vertexPosBuffer = Renderer::createDynamicVertexBuffer(VertexAttribType::FLOAT, VertexAttribSize::THREE, sizeof(glm::vec3) * m_vertices.size());
        m_textureCoordsBuffer = Renderer::createStaticVertexBuffer(m_textureCoords);
        m_indexBuffer = Renderer::createStaticIndexBuffer(m_indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        updateBuffersWithPositions();
    }

    void GUIObject::setPositionInPercents(const glm::vec3& pos) // Left bottom corner.
    {
        BR_ASSERT((pos.z >= 0.0f && pos.z <= 1.0f), "%s", "pos Z must be between 0 and 1.");

        m_positionInPercents = pos;
        // Recalculate only X and Y. Z will always in range 0...1.
        m_positionNormalized.x = m_positionInPercents.x / 100.0f;
        m_positionNormalized.y = m_positionInPercents.y / 100.0f;
        m_positionNormalized.z = m_positionInPercents.z;

        m_positionInPixels.x = m_positionNormalized.x * Window::getInstance()->getScreenWidth();
        m_positionInPixels.y = m_positionNormalized.y * Window::getInstance()->getScreenHeight();
        m_positionInPixels.z = m_positionInPercents.z;
    }

    void GUIObject::setWidthHeightInPercents(const glm::vec2& wh)
    {
        BR_ASSERT((wh.x >= 0.0f && wh.y >= 0.0f), "%s", "wh must be > 0.0f.");

        m_widthHeightInPercents = wh;
        m_widthHeightNormalized = m_widthHeightInPercents / 100.0f;
        m_widthHeightInPixels.x = m_widthHeightNormalized.x * Window::getInstance()->getScreenWidth();
        m_widthHeightInPixels.y = m_widthHeightNormalized.y * Window::getInstance()->getScreenHeight();
    }

    void GUIObject::updateBuffersWithPositions()
    {
        BR_ASSERT(m_vertexPosBuffer != nullptr, "%s", "Buffer is nullptr.");

        // Move coords to GUI screenSpace -1...1.
        glm::vec3 screenSpacePos = getPositionNormalized();
        screenSpacePos.x = screenSpacePos.x * 2.0f - 1.0f;
        screenSpacePos.y = screenSpacePos.y * 2.0f - 1.0f;
        glm::vec2 WH = getWidthHeightNormalized() * 2.0f;
        m_vertices[0].x = screenSpacePos.x;         m_vertices[0].y =  screenSpacePos.y;        m_vertices[0].z =screenSpacePos.z;
        m_vertices[1].x = screenSpacePos.x + WH.x;  m_vertices[1].y =  screenSpacePos.y;        m_vertices[1].z =screenSpacePos.z;
        m_vertices[2].x = screenSpacePos.x + WH.x;  m_vertices[2].y =  screenSpacePos.y + WH.y; m_vertices[2].z =screenSpacePos.z;
        m_vertices[3].x = screenSpacePos.x;         m_vertices[3].y =  screenSpacePos.y + WH.y; m_vertices[3].z =screenSpacePos.z;

        m_vertexPosBuffer->setDynamicBufferData(m_vertices, m_vertices.size());
    }
}
