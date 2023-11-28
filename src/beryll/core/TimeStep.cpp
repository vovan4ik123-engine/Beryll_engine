#include "TimeStep.h"

namespace Beryll
{
    uint64_t TimeStep::m_milliSecFromStart = 0;
    float TimeStep::m_secFromStart = 0.0f;

    uint64_t TimeStep::m_currentStepStart = 0;
    uint64_t TimeStep::m_timeStepMilliSec = 0;
    float TimeStep::m_timeStepSec = 0.0f;
}
