#include "MainImGUI.h"
#include "Beryll/Platform/AndroidGLES/AndroidGLESImGUI.h"
#include "Beryll/Core/GameLoop.h"
#include "Beryll/Core/Log.h"

namespace Beryll
{
    std::unique_ptr<MainImGUI> MainImGUI::m_imGUI = nullptr;

    void MainImGUI::create()
    {
        if(m_imGUI) { return; }

        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            m_imGUI = std::unique_ptr<MainImGUI>(new AndroidGLESImGUI());
        }
        else
        {
            BR_ASSERT(false, "Can not create ImGUI. Unknown platform.");
        }
    }
}