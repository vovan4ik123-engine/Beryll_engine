#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class TimeStep
    {
    public:
        static uint32_t getMillisecFromStart()
        {
            return SDL_GetTicks();
        }

        static float getSecFromStart()
        {
            return static_cast<float>(getMillisecFromStart()) / 1000.0f;
        }

        static void sleep(uint32_t millisec)
        {
            SDL_Delay(millisec);
        }

        static void fixateTime()
        {
            m_timeStepMIlliSec = getMillisecFromStart() - m_currentStepStart;
            m_timeStepSec = m_timeStepMIlliSec / 1000.0f;

            m_currentStepStart = getMillisecFromStart();
        }

        static uint32_t getTimeStepMilliSec()
        {
            return m_timeStepMIlliSec;
        }

        static float getTimeStepSec()
        {
            return m_timeStepSec;
        }

    private:
        static uint32_t m_currentStepStart; // time in millisec
        static uint32_t m_timeStepMIlliSec; // time between two last fixateTime() call in millisec
        static float m_timeStepSec; // time between two last fixateTime() call in sec
    };
}