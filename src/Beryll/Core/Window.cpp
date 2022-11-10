#include "Window.h"
#include "Beryll/Platform/AndroidGLES/AndroidGLESWindow.h"

namespace Beryll
{
    std::unique_ptr<Window> Window::m_window = nullptr;

    void Window::create()
    {
        if(m_window) { return; }

        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            m_window = std::unique_ptr<Window>(new AndroidGLESWindow());
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create Window. Unknown platform.");
        }
    }
}