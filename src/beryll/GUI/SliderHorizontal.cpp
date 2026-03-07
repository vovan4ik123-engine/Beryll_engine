#include "SliderHorizontal.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    SliderHorizontal::SliderHorizontal(const char* sliderTrackTexturePath,
                                       const char* sliderThumbTexturePath,
                                       const glm::vec3& pos, const glm::vec2& widthHeight,
                                       float minValue, float maxValue)
                                       : GUIObject(pos, widthHeight), m_min(minValue), m_max(maxValue), m_sliderValue(minValue)
    {
        BR_ASSERT((sliderTrackTexturePath != nullptr && sliderTrackTexturePath[0] != '\0'), "%s", "Path to slider track can not be empty.");
        BR_ASSERT((sliderThumbTexturePath != nullptr && sliderThumbTexturePath[0] != '\0'), "%s", "Path to slider thumb can not be empty.");
        BR_ASSERT((m_min < m_max), "%s", "Slider min value must be less than max value.");



    }

    SliderHorizontal::~SliderHorizontal()
    {

    }

    void SliderHorizontal::updateBeforePhysics()
    {
        m_valueChanging = false;

        std::vector<Finger>& fingers = EventHandler::getFingers();

        if(m_fingerIDDownEvent != -1)
        {
            bool touchedFingerStillOnScreen = false;
            for(Finger& f : fingers)
            {
                if(m_fingerIDDownEvent == f.ID)
                {
                    touchedFingerStillOnScreen = true;
                }
            }

            if(!touchedFingerStillOnScreen)
                m_fingerIDDownEvent = -1;
        }

        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > getPositionNormalized().x && f.normalizedPos.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
               f.normalizedPos.y > getPositionNormalized().y && f.normalizedPos.y < getPositionNormalized().y + getWidthHeightNormalized().y)
            {
                // If any finger in slider area.
                if(f.downEvent && !f.handled)
                {
                    m_fingerIDDownEvent = f.ID;
                    f.handled = true;
                }
            }

            // Touched finger still on screen.
            if(m_fingerIDDownEvent != -1 && f.ID == m_fingerIDDownEvent)
            {
                // Calculate position on slider
                float fingerXPos = f.normalizedPos.x;
                if(fingerXPos <= getPositionNormalized().x)
                    fingerXPos = getPositionNormalized().x;
                if(fingerXPos >= getPositionNormalized().x + getWidthHeightNormalized().x)
                    fingerXPos = getPositionNormalized().x + getWidthHeightNormalized().x;

                float normalizedSliderProgress = (fingerXPos - getPositionNormalized().x) / getWidthHeightNormalized().x;
                if(normalizedSliderProgress < 0.0f)
                    normalizedSliderProgress = 0.0f;
                if(normalizedSliderProgress > 1.0f)
                    normalizedSliderProgress = 1.0f;

                float valueRange = m_max - m_min;
                m_sliderValue = m_min + (valueRange * normalizedSliderProgress);
                m_valueChanging = true;

                return;
            }
        }
    }

    void SliderHorizontal::updateAfterPhysics()
    {

    }

    void SliderHorizontal::draw()
    {

    }
}
