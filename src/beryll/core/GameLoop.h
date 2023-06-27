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

        static void setFPSMaxLimit(float fps)
        {
            if(m_FPSMaxLimit > m_FPSMinLimit)
                m_FPSMaxLimit = fps;
        }

        static float getFPS() { return 1000000.0f / m_frameTimeIncludeSleep; };

        static float getFrameTime() { return m_frameTime * 0.001; } // Convert to milliSec.
        static float getCPUTime() { return m_CPUTime * 0.001; } // All logic + Physics simulation // convert to milliSec.
        static float getGPUTime() { return m_GPUTime * 0.001; } // Draw calls + GPU synchronization // convert to milliSec.
        static void stopLoop() { m_isRun = false; }

    private:
        friend int ::main(int argc, char *argv[]);

        static void create(ScreenOrientation orientation);
        static void run();
        static void regulateFPS();

        static bool m_isRun;

        static float m_loopTime; // In microSec.
        static float m_frameStart;
        static float m_frameTime;
        static float m_frameTimeIncludeSleep;
        static float m_CPUTime;
        static float m_GPUTimeStart;
        static float m_GPUTime;
        static Timer m_timer;

        static float m_FPS;
        static float m_FPSMaxLimit;
        static constexpr float m_FPSMinLimit = 1.0f;

        static int m_regulateFPSFramesCount;
        static float m_regulateFPSFramesSum;
    };
}
