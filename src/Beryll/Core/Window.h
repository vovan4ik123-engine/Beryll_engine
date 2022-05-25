#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/Core/Log.h"
#include "Beryll/Core/GameLoop.h"

namespace Beryll
{
    class Window
    {
    public:
        Window() {}
        virtual ~Window() {}

        Window(const Window& w) = delete;
        Window& operator=(const Window& w) = delete;

        Window(Window&& w) = delete;
        Window& operator=(Window&& w) = delete;

        static std::unique_ptr<Window>& getInstance()
        {
            BR_ASSERT((m_window != nullptr), "Window is not created. Use Window::create() first");
            return m_window;
        }

        virtual void reCreate() = 0;
        virtual void checkOrientationChange() = 0; // if phone or tablet screen rotation allowed

        virtual SDL_Window* getWindow() = 0;
        virtual uint32_t getScreenWidth() = 0;
        virtual uint32_t getScreenHeight() = 0;

        virtual void setClearColor(float r, float g, float b, float a) = 0;  // background color after clear()
        virtual void clear() = 0;
        virtual void finishDraw() = 0; // wait for finish all draw commands before swapWindow()
        virtual void flushDraw() = 0; // dont wait for finish draw commands before swapWindow()
        virtual void swapWindow() = 0;

        // OpenGL specific methods
        virtual SDL_GLContext* getGlContext() = 0;

        // Metal specific methods

    private:
        friend class GameLoop;
        static void create(); // should be called only in GameLoop

        static std::unique_ptr<Window> m_window;
    };
}