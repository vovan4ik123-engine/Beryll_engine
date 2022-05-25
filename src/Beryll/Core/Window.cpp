#include "Window.h"
#include "Beryll/Platform/AndroidGLES/AndroidGLESWindow.h"
#include "Beryll/Core/GameLoop.h"

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
            BR_ASSERT(false, "Can not create Window. Unknown platform.");
        }
    }
}