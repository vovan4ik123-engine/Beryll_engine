#include "TimeStep.h"

namespace Beryll
{
    unsigned long TimeStep::m_milliSecFromStart = 0;
    float TimeStep::m_secFromStart = 0.0f;

    unsigned long TimeStep::m_currentStepStart = 0;
    unsigned long TimeStep::m_timeStepMilliSec = 0;
    float TimeStep::m_timeStepSec = 0.0f;
}
