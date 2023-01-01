#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class GameLoop
    {
    public:
        GameLoop() = delete;
        ~GameLoop() = delete;

        // allowed FPS [5 ... 250]
        static void setMaxFPS(uint32_t fps)
        {
            if(fps >= 5 && fps <= 250)
            {
                m_maxFPS = fps;
                m_loopTime = 1000 / m_maxFPS;
            }
        }

        static uint32_t getFPS();
        static uint32_t getMaxFPS() { return m_maxFPS; }
        static uint32_t getFrameTime() { return m_frameTime; }
        static void stopLoop() { m_isRun = false; }

        // for statistics
        static uint32_t calcStart;
        static uint32_t calcTime;
        static uint32_t drawStart;
        static uint32_t drawTime;

    private:
        friend int ::main(int argc, char *argv[]);

        static void create();
        static void run();

        static bool m_isRun;

        static uint32_t m_loopTime; // in millisec
        static uint32_t m_frameStart;
        static uint32_t m_frameTime;

        static uint32_t m_maxFPS;
    };
}
