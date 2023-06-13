#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/GUI/MainImGUI.h"
#include "beryll/core/Log.h"

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
        * path must skip assets/ folder and starts from next folder inside assets/.
        * heightInPercentOfScreen - font height in percent 0%...100% of screen.
        */
        ImFont* createFont(const std::string& path, float heightInPercentOfScreen) override;
        void deleteFont(ImFont* font) override;
        void setDefaultFont(const std::string& path, float heightInPercentOfScreen) override;

    private:
        friend class MainImGUI;
        AndroidGLESImGUI();

        void init();
        void destroy();
    };
}
