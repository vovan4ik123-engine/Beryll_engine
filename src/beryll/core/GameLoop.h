#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "beryll/core/Timer.h"

namespace Beryll
{
    class GameLoop
    {
    public:
        GameLoop() = delete;
        ~GameLoop() = delete;

        // allowed FPS [5 ... 250]
        static void setMaxFPS(float fps, bool regulateFPS = true)
        {
            if(fps >= 5.0f && fps <= 250.0f)
            {
                m_maxFPS = fps;
                m_loopTime = 1000000.0f / m_maxFPS; // in microSec

                m_regulateFPS = regulateFPS;
            }
        }

        static float getFPS() { return 1000000.0f / m_frameTimeIncludeSleep; };
        static float getMaxFPS() { return m_maxFPS; }

        static float getFrameTime() { return m_frameTime * 0.001; } // in milliSec
        static float getCPUTime() { return m_CPUTime * 0.001; } // in milliSec
        static float getGPUTime() { return m_GPUTime * 0.001; } // draw calls + GPU synchronization // in milliSec
        static void stopLoop() { m_isRun = false; }

    private:
        friend int ::main(int argc, char *argv[]);

        static void create();
        static void run();
        static void regulateFPS();

        static bool m_isRun;

        static float m_loopTime; // in microSec
        static float m_frameStart;
        static float m_frameTime;
        static float m_frameTimeIncludeSleep;
        static float m_CPUTime;
        static float m_GPUTimeStart;
        static float m_GPUTime;
        static Timer m_timer;

        static float m_maxFPS;
        static bool m_regulateFPS;
        static int m_regulateFPSFramesCount;
        static float m_regulateFPSFramesSum;
    };
}
