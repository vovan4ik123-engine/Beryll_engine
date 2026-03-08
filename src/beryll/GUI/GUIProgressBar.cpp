#include "GUIProgressBar.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"

namespace Beryll
{
    GUIProgressBar::GUIProgressBar(const glm::vec3& pos, const glm::vec2& widthHeight,
                                   const glm::vec3& leftColor, const glm::vec3& rightColor) :
                                   GUIObject(pos, widthHeight), m_colorLeft(leftColor), m_colorRight(rightColor)
    {
        m_internalShader = Renderer::createShader(BeryllConstants::GUIProgressBarVertexPath.data(),
                                                  BeryllConstants::GUIProgressBarFragmentPath.data());
    }

    GUIProgressBar::~GUIProgressBar()
    {

    }

    void GUIProgressBar::updateBeforePhysics()
    {

    }

    void GUIProgressBar::updateAfterPhysics()
    {

    }

    void GUIProgressBar::draw()
    {
        // Move X position to GUI screenSpace -1...1.
        float XPos = getPositionNormalized().x;
        XPos = XPos * 2.0f - 1.0f;
        float W = getWidthHeightNormalized().x * 2.0f;

        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());
        m_internalShader->set3Float("colorLeft", m_colorLeft);
        m_internalShader->set3Float("colorRight", m_colorRight);
        m_internalShader->set1Float("progressXPos", (XPos + m_progress0to1 * W));

        m_vertexArray->bind();
        m_vertexArray->draw();
    }
}
