#include "Window.h"

#if defined(ANDROID)
    #include "beryll/platform/androidGLES/AndroidGLESWindow.h"
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

    void Window::setScreenOrientation(ScreenOrientation orientation)
    {
        if(orientation == ScreenOrientation::MOBILE_PORTRAIT)
        {
            SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");
        }
        else if(orientation == ScreenOrientation::MOBILE_LANDSCAPE_AND_FLIPPED)
        {
            SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
        }
        else if(orientation == ScreenOrientation::DESKTOP)
        {
            // Should work without changes.
        }
    }
}
