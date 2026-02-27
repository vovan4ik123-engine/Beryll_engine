#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class SliderHorizontal : public GUIObject
    {
    public:
        SliderHorizontal() = delete;
        /*
         * sliderTrackTexturePath - slider track.
         * sliderThumbTexturePath - slider movable element.
         * pos - X,Y in screen percents (0...100), Z in value as is (0...1).
         * widthHeight - width and height in screen percents (0...100).
         * minValue -
         * maxValue -
         */
        SliderHorizontal(const char* sliderTrackTexturePath,
                         const char* sliderThumbTexturePath,
                         const glm::vec3& pos, const glm::vec2& widthHeight,
                         float minValue, float maxValue);
        ~SliderHorizontal() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        float getValue() { return m_sliderValue; }
        float getMin() { return m_min; }
        float getMax() { return m_max; }
        bool getIsValueChanging() { return m_valueChanging; }
        void setValue(const float val)
        {
            if(val >= m_min && val <= m_max)
            {
                m_sliderValue = val;
            }
        }

    private:
        float m_min = 0.0f;
        float m_max = 1.0f;
        float m_sliderValue = 0.0f;
        bool m_valueChanging = false;
        long m_fingerIDDownEvent = -1;
    };
}
