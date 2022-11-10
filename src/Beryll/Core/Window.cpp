#include "Window.h"

#if defined(ANDROID)
    #include "Beryll/Platform/AndroidGLES/AndroidGLESWindow.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::unique_ptr<Window> Window::m_window = nullptr;

    void Window::create()
    {
        if(m_window) { return; }

#if defined(ANDROID)
        m_window = std::unique_ptr<Window>(new AndroidGLESWindow());
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create Window. Unknown platform.");
#endif
    }
}
