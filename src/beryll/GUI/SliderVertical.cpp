#include "SliderVertical.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    SliderVertical::SliderVertical(const char* sliderTrackTexturePath,
                                   const char* sliderThumbTexturePath,
                                   const glm::vec3& pos, const glm::vec2& widthHeight,
                                   float minValue, float maxValue, bool consumeDownEvent)
                                   : GUIObject(pos, widthHeight, consumeDownEvent), m_min(minValue), m_max(maxValue), m_sliderValue(minValue)
    {
        BR_ASSERT((sliderTrackTexturePath != nullptr && sliderTrackTexturePath[0] != '\0'), "%s", "Path to slider track can not be empty.");
        BR_ASSERT((sliderThumbTexturePath != nullptr && sliderThumbTexturePath[0] != '\0'), "%s", "Path to slider thumb can not be empty.");
        BR_ASSERT((m_min >= 0.0f && m_max >= 0.0f), "%s", "m_min >= 0.0f && m_max >= 0.0f.");
        BR_ASSERT((m_min < m_max), "%s", "Slider min value must be less than max value.");

        m_valueRange = m_max - m_min;

        m_trackTexture = Renderer::createTexture(sliderTrackTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_thumbTexture = Renderer::createTexture(sliderThumbTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_internalShader = Renderer::createShader(BeryllConstants::GUIElementWithTextureVertexPath.data(),
                                                  BeryllConstants::GUIElementWithTextureFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();

        m_trackPos = pos;
        m_trackPos.z = m_trackPos.z - 0.005f; // Make offset between slider track and thumb.
        m_trackWidthHeight = widthHeight;

        const float screenAR = Beryll::Window::getInstance()->getScreenAspectRatio();
        m_thumbWidthHeight = glm::vec2(widthHeight.x * 1.4f, widthHeight.x * 0.4f * screenAR);
        m_thumbPos = pos;
        m_thumbPos.x = m_thumbPos.x - ((m_thumbWidthHeight.x - widthHeight.x) * 0.5f);
        m_thumbPos.y = m_thumbPos.y - (m_thumbWidthHeight.y * 0.5f);
    }

    SliderVertical::~SliderVertical()
    {

    }

    void SliderVertical::updateBeforePhysics()
    {
        m_valueChanging = false;
        m_touchedFingerStillOnScreen = false;

        std::vector<Finger>& fingers = EventHandler::getFingers();

        for(Finger& f : fingers)
        {
            if(m_fingerIDDownEvent == f.ID)
            {
                m_touchedFingerStillOnScreen = true;
            }
        }

        if(!m_touchedFingerStillOnScreen)
            m_fingerIDDownEvent = -1;

        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > getPositionNormalized().x && f.normalizedPos.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
               f.normalizedPos.y > getPositionNormalized().y && f.normalizedPos.y < getPositionNormalized().y + getWidthHeightNormalized().y)
            {
                // If any finger in slider area.
                if(f.downEvent)
                {
                    m_fingerIDDownEvent = f.ID;
                    m_touchedFingerStillOnScreen = true;

                    if(m_consumeEvent)
                        f.downEvent = false;
                }
            }

            if(m_touchedFingerStillOnScreen && f.ID == m_fingerIDDownEvent)
            {
                // Calculate position on slider
                float fingerYPos = f.normalizedPos.y;
                if(fingerYPos <= getPositionNormalized().y)
                    fingerYPos = getPositionNormalized().y;
                if(fingerYPos >= getPositionNormalized().y + getWidthHeightNormalized().y)
                    fingerYPos = getPositionNormalized().y + getWidthHeightNormalized().y;

                m_normalizedSliderProgress = (fingerYPos - getPositionNormalized().y) / getWidthHeightNormalized().y;
                m_sliderValue = m_min + (m_valueRange * m_normalizedSliderProgress);
                m_valueChanging = true;

                return;
            }
        }
    }

    void SliderVertical::updateAfterPhysics()
    {

    }

    void SliderVertical::draw()
    {
        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());
        m_vertexArray->bind();

        // Draw thumb first. Be careful, It update position and size which are used in updateBeforePhysics().
        m_thumbTexture->bind();
        const float prPerc = getWidthHeightInPercents().y * m_normalizedSliderProgress; // Progress in screen percents.
        m_thumbPos.y = (m_trackPos.y + prPerc) - (m_thumbWidthHeight.y * 0.5f);
        updatePositionInPercents(m_thumbPos, false);
        updateWidthHeightInPercents(m_thumbWidthHeight, true);
        m_vertexArray->draw();

        // Draw track second. Be careful, It update position and size which are used in updateBeforePhysics().
        m_trackTexture->bind();
        updatePositionInPercents(m_trackPos, false);
        updateWidthHeightInPercents(m_trackWidthHeight, true);
        m_vertexArray->draw();
    }
}
