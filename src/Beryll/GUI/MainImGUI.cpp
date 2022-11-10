#include "MainImGUI.h"

#if defined(ANDROID)
    #include "Beryll/Platform/AndroidGLES/AndroidGLESImGUI.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::unique_ptr<MainImGUI> MainImGUI::m_imGUI = nullptr;

    void MainImGUI::create()
    {
        if(m_imGUI) { return; }

#if defined(ANDROID)
        m_imGUI = std::unique_ptr<MainImGUI>(new AndroidGLESImGUI());
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create ImGUI. Unknown platform.");
#endif
    }
}
