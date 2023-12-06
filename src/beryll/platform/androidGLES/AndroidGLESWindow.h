#pragma once

#include "LibsHeaders.h"

#include "beryll/core/Window.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    class AndroidGLESWindow : public Window
    {
    public:
        ~AndroidGLESWindow() override;

        void reCreate() override;
        void checkOrientationChange() override; // If phone or tablet screen rotation allowed.

        SDL_Window* getWindow() override { return m_window; }
        SDL_GLContext* getGlContext() override { return &m_glContext; }
        int getScreenWidth() override { return m_screenWidth;}
        int getScreenHeight() override { return m_screenHeight;}
        int getMaxSupportedTextureResolution() override { return m_maxSupportedTextureResolution;}
        int getMaxSupportedViewportResolution() override { return m_maxSupportedViewportResolution;}
        float getScreenAspectRation() override { return m_screenAspectRation; }

        void setClearColor(float r, float g, float b, float a) override { glClearColor(r, g, b, a); }
        void clear() override { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); } // GL_STENCIL_BUFFER_BIT GL_ACCUM_BUFFER_BIT
        void finishDraw() override { glFinish(); } // Wait for finish all draw commands before swapWindow().
        void flushDraw() override { glFlush(); } // Dont wait for finish draw commands before swapWindow().
        void swapWindow() override { SDL_GL_SwapWindow(m_window); }

    private:
        friend class Window;
        AndroidGLESWindow();

        SDL_Window* m_window = nullptr;
        SDL_GLContext m_glContext = nullptr;

        int m_screenWidth = 0;
        int m_screenHeight = 0;
        int m_maxSupportedTextureResolution = 0;
        int m_maxSupportedViewportResolution = 0;
        float m_screenAspectRation = 0.0f;
    };
}
