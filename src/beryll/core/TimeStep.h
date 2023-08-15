#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class TimeStep final
    {
    public:
        static unsigned long getMilliSecFromStart()
        {
            return m_milliSecFromStart;
        }

        static float getSecFromStart()
        {
            return m_secFromStart;
        }

        static unsigned int getTimeStepMilliSec()
        {
            return m_timeStepMilliSec;
        }

        static float getTimeStepSec()
        {
            return m_timeStepSec;
        }

    private:
        static unsigned long m_milliSecFromStart; // Time in milliSec passed after application start.
        static float m_secFromStart; // Time in sec passed after application start.

        static unsigned long m_currentStepStart; // Time in milliSec.
        static unsigned long m_timeStepMilliSec; // Time between two last fixateTime() call in milliSec.
        static float m_timeStepSec; // Time between two last fixateTime() call in sec.

        friend class GameLoop;
        // Should be called inside GameLoop before every frame start.
        static void fixateTime()
        {
            m_milliSecFromStart = SDL_GetTicks64();
            m_secFromStart = static_cast<float>(m_milliSecFromStart) * 0.001f; // Same as / 1000.0f.

            m_timeStepMilliSec = m_milliSecFromStart - m_currentStepStart;
            m_timeStepSec = static_cast<float>(m_timeStepMilliSec) * 0.001f; // Same as / 1000.0f.

            m_currentStepStart = m_milliSecFromStart;
        }
    };
}
