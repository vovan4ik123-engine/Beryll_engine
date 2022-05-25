#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/GUI/MainImGUI.h"
#include "Beryll/Core/Log.h"

namespace Beryll
{
    class AndroidGLESImGUI : public MainImGUI
    {
    public:
        ~AndroidGLESImGUI() override;

        void reCreate() override;

        void beginFrame() override;
        void endFrame() override;

        float getGUIWidth() override;
        float getGUIHeight() override;

        /*
        * path must skip assets/ folder and starts from next folder inside assets/
        * heightPercent - font height in percent 0%...100% of screen
        */
        void setDefaultFont(const char* path, float heightInPercent) override;
        void setButtonsFont(const char* path, float heightInPercent) override;
        void setCheckBoxesFont(const char* path, float heightInPercent) override;
        void setTextsFont(const char* path, float heightInPercent) override;

    private:
        friend class MainImGUI;
        AndroidGLESImGUI();

        void init();
        void destroy();

        std::string m_defaultFontPath;
        float m_defaultFontHeight = 0.0f; // in range 0 = 0%...1 = 100% of screen size
    };
}