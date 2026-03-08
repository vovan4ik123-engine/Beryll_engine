#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class GUIProgressBar : public GUIObject
    {
    public:
        GUIProgressBar() = delete;
        /*
         * pos - X,Y in screen percents (0...100), Z in value as is (0...1).
         * widthHeight - width and height in screen percents (0...100).
         * leftColor
         * rightColor
         */
        GUIProgressBar(const glm::vec3& pos, const glm::vec2& widthHeight,
                       const glm::vec3& leftColor, const glm::vec3& rightColor);
        ~GUIProgressBar() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void setProgress(const float progress0to1)
        {
            m_progress0to1 = progress0to1;
            if(m_progress0to1 < 0.0f) m_progress0to1 = 0.0f;
            if(m_progress0to1 > 1.0f) m_progress0to1 = 1.0f;
        }

    private:
        float m_progress0to1 = 0.0f; // In range 0...1.
        glm::vec3 m_colorLeft{1.0f, 0.0f, 0.0f};
        glm::vec3 m_colorRight{0.0f, 1.0f, 0.0f};

        // Vertex and index buffers are in base class.
        // ........
    };
}
