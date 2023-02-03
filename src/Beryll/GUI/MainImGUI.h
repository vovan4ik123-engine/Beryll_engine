#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/Core/Log.h"
#include "Beryll/Core/GameLoop.h"

namespace Beryll
{
    // for all ImGUI elements
    // call MainImGUI::getInstance()->beginFrame() after Window::getInstance()->clear()
    // and before .draw() methods with any ImGUI element
    // call MainImGUI::getInstance()->endFrame() after .draw() methods with any ImGUI element
    // and before Window::getInstance()->finishDraw() and Window::getInstance()->swapWindow()
    class MainImGUI
    {
    public:
        virtual ~MainImGUI() {}

        static std::unique_ptr<MainImGUI>& getInstance()
        {
            BR_ASSERT((m_imGUI != nullptr), "%s", "MainImGUI is not created. Use MainImGUI::create() first");
            return m_imGUI;
        }

        virtual void reCreate() = 0;

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual float getGUIWidth() = 0;
        virtual float getGUIHeight() = 0;

        /*
        * path must skip assets/ folder and starts from next folder inside assets/
        * heightPercent - font height in percent 0%...100% of screen
        */
        virtual void setDefaultFont(const char* path, float heightInPercent) = 0;
        virtual void setDefaultFontHeight(float heightInPercent) = 0;
        virtual void setButtonsFont(const char* path, float heightInPercent) = 0;
        virtual void setCheckBoxesFont(const char* path, float heightInPercent) = 0;
        virtual void setTextsFont(const char* path, float heightInPercent) = 0;
        virtual void setSlidersFont(const char* path, float heightInPercent) = 0;

    protected:
        MainImGUI() {}
        std::string m_defaultFontPath;
        float m_defaultFontHeight = 3.0f / 100.0f; // in range 0...1 (0%...100% of screen size). 3.0f / 100.0f = 3% of screen

    private:
        friend class GameLoop;
        static void create();

        static std::unique_ptr<MainImGUI> m_imGUI;
    };
}
