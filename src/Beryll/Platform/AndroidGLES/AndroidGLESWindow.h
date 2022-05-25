#pragma once

#include "LibsHeaders.h"

#include "Beryll/Core/Window.h"
#include "Beryll/Core/Log.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    class AndroidGLESWindow : public Window
    {
    public:
        ~AndroidGLESWindow() override;

        void reCreate() override;
        void checkOrientationChange() override; // if phone or tablet screen rotation allowed

        SDL_Window* getWindow() override { return m_window; }
        SDL_GLContext* getGlContext() override { return &m_glContext; }
        uint32_t getScreenWidth() override { return m_screenWidth;}
        uint32_t getScreenHeight() override { return m_screenHeight;}

        void setClearColor(float r, float g, float b, float a) override
        {
            glClearColor(r, g, b, a);
        }
        void clear() override
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //	| GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
        }
        void finishDraw() override // wait for finish all draw commands before swapWindow()
        {
            glFinish();
        }
        void flushDraw() override // dont wait for finish draw commands before swapWindow()
        {
            glFlush();
        }
        void swapWindow() override
        {
            SDL_GL_SwapWindow(m_window);
        }

    private:
        friend class Window;
        AndroidGLESWindow();

        SDL_DisplayMode m_DM;
        SDL_Window* m_window = nullptr;
        SDL_GLContext m_glContext = nullptr;

        uint32_t m_screenWidth = 0;
        uint32_t m_screenHeight = 0;
    };
}
