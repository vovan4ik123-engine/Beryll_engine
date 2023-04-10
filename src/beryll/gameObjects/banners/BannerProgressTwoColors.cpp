#include "BannerProgressTwoColors.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"

namespace Beryll
{
    BannerProgressTwoColors::BannerProgressTwoColors(float width, float height, const glm::vec3& colorLeft, const glm::vec3& colorRight)
    {
        m_width = width;
        m_height = height;
        m_colorLeft = colorLeft;
        m_colorRight = colorRight;
        m_leftXPos = 0.0f - m_width * 0.5f;
        m_RightXPos = 0.0f + m_width * 0.5f;

        std::vector<glm::vec3> vertices{ {0.0f - m_width * 0.5f, 0.0f - m_height * 0.5f, 0.0f},
                                         {0.0f + m_width * 0.5f, 0.0f - m_height * 0.5f, 0.0f},
                                         {0.0f + m_width * 0.5f, 0.0f + m_height * 0.5f, 0.0f},
                                         {0.0f - m_width * 0.5f, 0.0f + m_height * 0.5f, 0.0f} };
        m_vertexPosBuffer = Renderer::createStaticVertexBuffer(vertices);

        // Counter clockwise winding order
        std::vector<uint32_t> indices {0, 1, 3,
                                       1, 2, 3};
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader(BeryllConstants::bannerTwoColorsProgressVertexPath.data(),
                                                 BeryllConstants::bannerTwoColorsProgressFragmentPath.data());
    }

    BannerProgressTwoColors::~BannerProgressTwoColors()
    {

    }

    void BannerProgressTwoColors::draw()
    {
        BR_ASSERT(((progress >= 0.0f) && (progress <= 1.0f)), "%s", "Progress should be in range 0.0f...1.0f")

        m_internalShader->bind();
        m_MVP = Camera::getViewProjection() * m_modelMatrix;
        m_internalShader->setMatrix4x4Float("MVPMatrix", m_MVP);

        m_internalShader->set3Float("colorLeft", m_colorLeft);
        m_internalShader->set3Float("colorRight", m_colorRight);
        m_internalShader->set1Float("progressXPos", m_leftXPos + (progress * m_width));

        m_vertexArray->bind();
        m_vertexArray->draw();
    }
}