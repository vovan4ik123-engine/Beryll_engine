#include "AndroidGLESWindow.h"
#include "beryll/core/EventHandler.h"
#include "beryll/GUI/MainImGUI.h"

namespace Beryll
{
    AndroidGLESWindow::AndroidGLESWindow()
    {
        if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_SENSOR) < 0)
        {
            BR_ASSERT(false, "%s", "SDL init error.");
        }

        // SET ATTRIBUTE ONLY after initialize.
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); // Set to 1 to require hardware acceleration.
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Minimum number of bits for the red channel.
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); // Minimum number of bits for the green channel.
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8); // Minimum number of bits for the blue channel.
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // Minimum number of bits in depth buffer; default 16.

        // SDL antialiasing for real device. Dont work on emulator !!
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // Enable antialiasing sdl.
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // 0 4 8

        // Fill native display resolution.
        //SDL_GetDesktopDisplayMode(0, &m_DM);
        // Fill new resolution if resolution was changed in fullscreen mode.
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        m_screenWidth = DM.w;
        m_screenHeight = DM.h;

        // Create an application window with the following settings:
        m_window = SDL_CreateWindow("MainWindow",
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    m_screenWidth, m_screenHeight,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN |
                                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

        if(m_window == nullptr)
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

            m_window = SDL_CreateWindow("MainWindow",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        m_screenWidth, m_screenHeight,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN |
                                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        }

        BR_ASSERT((m_window != nullptr), "%s", "m_window == nullptr");

        m_glContext = SDL_GL_CreateContext(m_window);
        SDL_GL_MakeCurrent(m_window, m_glContext);

        SDL_GL_SetSwapInterval(0); // Disable vsync because increase time of SDL_GL_SwapWindow().
        //int vsync = SDL_GL_SetSwapInterval(-1); // Enable adaptive vsync.
        //if(vsync == -1) { SDL_GL_SetSwapInterval(1); } // Enable standard vsync.

        glViewport(0, 0, m_screenWidth, m_screenHeight);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Src alpha for src color, 1 - src alpha for destination color.

        if(m_screenWidth > m_screenHeight)
            m_screenAspectRation = float(m_screenWidth) / float(m_screenHeight);
        else
            m_screenAspectRation = float(m_screenHeight) / float(m_screenWidth);

        int check = 0;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &check);
        BR_INFO("GL_MAX_TEXTURE_IMAGE_UNITS: %d", check);

        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &check);
        BR_INFO("GL_MAX_RENDERBUFFER_SIZE: %d", check);

        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &m_maxSupportedViewportResolution);
        BR_INFO("GL_MAX_VIEWPORT_DIMS: %d", m_maxSupportedViewportResolution);

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxSupportedTextureResolution);
        BR_INFO("GL_MAX_TEXTURE_SIZE: %d", m_maxSupportedTextureResolution);

        BR_INFO("AndroidGLESWindow created and initialized. width: %d, height: %d", m_screenWidth, m_screenHeight);
    }

    AndroidGLESWindow::~AndroidGLESWindow()
    {
        SDL_GL_DeleteContext(m_glContext);
        SDL_DestroyWindow(m_window);
        SDL_Quit();

        BR_INFO("%s", "AndroidGLESWindow destroyed + SDL_Quit");
    }

    void AndroidGLESWindow::reCreate()
    {
        SDL_DestroyWindow(m_window);

        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        m_screenWidth = DM.w;
        m_screenHeight = DM.h;

        //SDL_GetDesktopDisplayMode(0, &m_DM); // Native display resolution.

        m_window = SDL_CreateWindow("MainWindow",
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    m_screenWidth, m_screenHeight,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);

        BR_ASSERT((m_window != nullptr), "%s", "m_window == nullptr");

        SDL_GL_MakeCurrent(m_window, m_glContext); // Set old context for new window.

        SDL_GL_SetSwapInterval(0); // Disable vsync because increase time of SDL_GL_SwapWindow().
        //int vsync = SDL_GL_SetSwapInterval(-1); // Enable adaptive vsync.
        //if(vsync == -1) { SDL_GL_SetSwapInterval(1); } // Enable standard vsync.

        glViewport(0, 0, m_screenWidth, m_screenHeight); // For current resolution.
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if(m_screenWidth > m_screenHeight)
            m_screenAspectRation = float(m_screenWidth) / float(m_screenHeight);
        else
            m_screenAspectRation = float(m_screenHeight) / float(m_screenWidth);

        BR_INFO("AndroidGLESWindow re created. width: %d, height: %d", m_screenWidth, m_screenHeight);
    }

    void AndroidGLESWindow::checkOrientationChange()
    {
        if(EventHandler::checkEvent(EventID::DISPLAY_ORIENTATION_CHANGE))
        {
            EventHandler::resetEvents(EventID::DISPLAY_ORIENTATION_CHANGE);
            //Window::getInstance()->reCreate();
            //MainImGUI::getInstance()->reCreate();
        }
    }
}
