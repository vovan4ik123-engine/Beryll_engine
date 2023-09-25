#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Log.h"

namespace Beryll
{

    enum class ScreenOrientation
    {
        // Use only on mobile.
        MOBILE_PORTRAIT,
        MOBILE_LANDSCAPE_AND_FLIPPED,

        // Use for desktop.
        DESKTOP
    };

    class Window
    {
    public:
        virtual ~Window() {}

        static std::unique_ptr<Window>& getInstance()
        {
            BR_ASSERT((m_window != nullptr), "%s", "Window is not created. Use Window::create() first.");
            return m_window;
        }

        virtual void reCreate() = 0;
        virtual void checkOrientationChange() = 0; // If phone or tablet screen rotation allowed.

        virtual SDL_Window* getWindow() = 0;
        virtual int getScreenWidth() = 0;
        virtual int getScreenHeight() = 0;


        virtual void setClearColor(float r, float g, float b, float a) = 0;  // Background color after clear().
        virtual void clear() = 0;
        virtual void finishDraw() = 0; // Wait for finish all draw commands before swapWindow().
        virtual void flushDraw() = 0; // Dont wait for finish draw commands before swapWindow().
        virtual void swapWindow() = 0;

        // OpenGL specific methods.
        virtual SDL_GLContext* getGlContext() = 0;

        // Metal specific methods.

        SDL_DisplayOrientation currentOrientation = SDL_ORIENTATION_UNKNOWN;

    protected:
        Window() {}

    private:
        friend class GameLoop;
        static void create(); // Should be called only in GameLoop.
        static void setScreenOrientation(ScreenOrientation orientation);

        static std::unique_ptr<Window> m_window;
    };
}
