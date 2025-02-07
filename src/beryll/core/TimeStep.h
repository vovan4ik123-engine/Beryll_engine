#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class TimeStep final
    {
    public:
        static uint64_t getMilliSecFromStart()
        {
            return m_milliSecFromStart;
        }

        static float getSecFromStart()
        {
            return m_secFromStart;
        }

        static uint64_t getTimeStepMilliSec()
        {
            return m_timeStepMilliSec;
        }

        static float getTimeStepSec()
        {
            return m_timeStepSec;
        }

        static uint64_t getSecSinceEpoch()
        {
            uint64_t secSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            return secSinceEpoch;
        }

        // Should be called inside GameLoop before every frame start.
        static void fixateTime()
        {
            m_milliSecFromStart = SDL_GetTicks();
            m_secFromStart = static_cast<float>(m_milliSecFromStart) * 0.001f; // Same as / 1000.0f.

            m_timeStepMilliSec = m_milliSecFromStart - m_currentStepStart;
            m_timeStepSec = static_cast<float>(m_timeStepMilliSec) * 0.001f; // Same as / 1000.0f.

            m_currentStepStart = m_milliSecFromStart;
        }

    private:
        static uint64_t m_milliSecFromStart; // Time in milliSec passed after application start.
        static float m_secFromStart; // Time in sec passed after application start.

        static uint64_t m_currentStepStart; // Time in milliSec.
        static uint64_t m_timeStepMilliSec; // Time between two last fixateTime() call in milliSec.
        static float m_timeStepSec; // Time between two last fixateTime() call in sec.
    };
}
