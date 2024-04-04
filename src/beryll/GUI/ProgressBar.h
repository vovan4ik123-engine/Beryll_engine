#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class ProgressBar : public GUIObject
    {
    public:
        ProgressBar() = delete;
        // Position and size in percent 0...100% of screen.
        ProgressBar(float left, float top, float width, float height);
        ~ProgressBar() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        float getProgress() { return m_progress; }
        void setProgress(float progress)
        {
            m_progress = progress;

            if(progress < 0.0f)
                m_progress = 0.0f;
            else if(progress > 1.0f)
                m_progress = 1.0f;
        }

        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }
        void setProgressColor(float r, float g, float b, float a) { m_progressColor = ImVec4{ r, g, b, a }; }
        void setBackgroundColor(float r, float g, float b, float a) { m_backgroundColor = ImVec4{ r, g, b, a }; }

    private:
        float m_progress = 0.0f; // In range 0.0f...1.0f.

        ImVec4 m_fontColor { 0.0f, 0.0f, 0.0f, 1.0f }; // 0.0f - 1.0f range.
        ImVec4 m_progressColor { 0.0f, 1.0f, 0.0f, 1.0f };
        ImVec4 m_backgroundColor { 1.0f, 0.0f, 0.0f, 1.0f };
    };
}
