#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/Core/Log.h"

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
        MainImGUI() {}
        virtual ~MainImGUI() {}

        MainImGUI(const MainImGUI& w) = delete;
        MainImGUI& operator=(const MainImGUI& w) = delete;

        MainImGUI(MainImGUI&& w) = delete;
        MainImGUI& operator=(MainImGUI&& w) = delete;

        static std::unique_ptr<MainImGUI>& getInstance()
        {
            BR_ASSERT((m_imGUI != nullptr), "MainImGUI is not created. Use MainImGUI::create() first");
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
        std::string m_defaultFontPath;
        float m_defaultFontHeight = 0.03f; // in range 0...1 (0%...100% of screen size). 0.03f = 3% of screen

    private:
        friend class GameLoop;
        static void create();

        static std::unique_ptr<MainImGUI> m_imGUI;
    };
}
