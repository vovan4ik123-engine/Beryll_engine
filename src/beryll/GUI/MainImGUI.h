#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Log.h"

namespace Beryll
{
    struct FontsLoadedInImFontAtlas
    {
        FontsLoadedInImFontAtlas(ImFont* f, std::string p, float h)
        : font(f), path(std::move(p)), heightInPercent(h) {}

        ImFont* font = nullptr;

        std::string path;
        float heightInPercent = 0.0f; // In range 0...100
    };

    // For all ImGUI elements.
    // Call MainImGUI::getInstance()->beginFrame() after Window::getInstance()->clear()
    // and before .draw() methods with any ImGUI element.
    // Call MainImGUI::getInstance()->endFrame() after .draw() methods with any ImGUI element
    // and before Window::getInstance()->finishDraw() and Window::getInstance()->swapWindow().
    class MainImGUI
    {
    public:
        MainImGUI() {}
        virtual ~MainImGUI() {}

        static std::unique_ptr<MainImGUI>& getInstance()
        {
            BR_ASSERT((m_imGUI != nullptr), "%s", "MainImGUI is not created. Use MainImGUI::create() first.");
            return m_imGUI;
        }

        virtual void reCreate() = 0;

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual float getGUIWidth() = 0;
        virtual float getGUIHeight() = 0;

        /*
        * path must skip assets/ folder and starts from next folder inside assets/.
        * heightInPercentOfScreen - font height in percent 0%...100% of screen.
        */
        virtual ImFont* createFont(const std::string& path, float heightInPercentOfScreen) = 0;
        virtual void deleteFont(ImFont* font) = 0;
        virtual void setDefaultFont(const std::string& path, float heightInPercentOfScreen) = 0;

    protected:
        // Store all loaded fonts in vector and check this vector before create new font.
        // Should be cleared together with call ImGui::GetIO().Fonts->Clear();
        static std::vector<FontsLoadedInImFontAtlas> m_loadedFonts;

        static ImFont* findStoredFont(const std::string& path, const float heightInPercent)
        {
            for(const FontsLoadedInImFontAtlas& fontData : m_loadedFonts)
            {
                if(fontData.path == path && fontData.heightInPercent == heightInPercent)
                    return fontData.font;
            }

            return nullptr;
        }

    private:
        friend class GameLoop;
        static void create();

        static std::unique_ptr<MainImGUI> m_imGUI;
    };
}
