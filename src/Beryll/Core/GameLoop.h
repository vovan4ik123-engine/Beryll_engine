#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "Beryll/Core/TimeStep.h"

namespace Beryll
{
    enum class Platform
    {
        UNKNOWN,
        ANDROID_GLES // OpenGL is right-handed coordinate system in object space and world space.
    };

    class GameLoop
    {
    public:
        GameLoop() = delete;
        ~GameLoop() = delete;

        static void setMaxFPS(uint32_t fps)
        {
            if(fps >= 5 && fps <= 200)
            {
                m_maxFPS = fps;
                m_loopTime = 1000 / m_maxFPS;
            }
        }

        static uint32_t getFPS() { return 1000 / TimeStep::getTimeStepMilliSec(); }

        static uint32_t getMaxFPS() { return m_maxFPS; }

        static uint32_t getFrameTime() { return m_frameTime; }

        static Platform getPlatform() { return m_platform; }

        static void stopLoop() { m_isRun = false; }

    private:
        friend int ::main(int argc, char *argv[]);

        static void create(Platform p);
        static void run();

        static Platform m_platform;
        static bool m_isRun;

        static uint32_t m_loopTime; // in millisec
        static uint32_t m_frameStart;
        static uint32_t m_frameTime;

        static uint32_t m_maxFPS;
    };
}
