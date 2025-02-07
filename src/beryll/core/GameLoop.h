#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Timer.h"
#include "beryll/core/Window.h"

namespace Beryll
{
    class GameLoop final
    {
    public:
        GameLoop() = delete;
        ~GameLoop() = delete;

        static void setFPSLimit(float fps)
        {
            if(fps < 2.0f) { return; }

            m_FPS = fps;
            m_loopTimeMicroSec = 1000000.0f / m_FPS;
        }

        static float getFPS() { return 1000000.0f / m_frameTimeIncludeSleep; }

        static float getFrameTime() { return m_frameTime * 0.001f; } // All frame time. Convert to milliSec.
        static float getCPUTime() { return m_CPUTime * 0.001f; } // All logic + Physics simulation. Convert to milliSec.
        static float getGPUTime() { return m_GPUTime * 0.001f; } // Draw calls + GPU synchronization. Convert to milliSec.
        static void stopLoop() { m_isRun = false; }

        static bool needDraw;

    private:
        friend int ::main(int argc, char* argv[]);

        static void create(ScreenOrientation orientation);
        static void run();

        static bool m_isRun;

        static float m_frameStart;
        static float m_frameTime;
        static float m_frameTimeIncludeSleep;
        static float m_CPUTime;
        static float m_GPUTimeStart;
        static float m_GPUTime;
        static Timer m_timer;

        static float m_FPS;
        static float m_loopTimeMicroSec;
        static long long int m_sleepTimeMicroSec;
    };
}
