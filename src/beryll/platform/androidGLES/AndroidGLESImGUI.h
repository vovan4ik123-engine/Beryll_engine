#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/GUI/MainImGUI.h"

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
        float getGUIScreenAspectRation() override;

        /*
        * path must skip assets/ folder and starts from next folder inside assets/.
        * fontHeight - font height in range 0...1.
        */
        ImFont* createFont(const std::string& path, float fontHeight) override;
        void deleteFont(ImFont* font) override;
        void setDefaultFont(const std::string& path, float fontHeight) override;

    private:
        friend class MainImGUI;
        AndroidGLESImGUI();

        void init();
        void destroy();
    };
}
