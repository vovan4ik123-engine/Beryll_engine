#include "AsyncRun.h"

namespace Beryll
{
    const uint32_t AsyncRun::m_numThreads = std::max(std::thread::hardware_concurrency() - 1, 1u);
    std::vector<std::future<void>> AsyncRun::m_futuresVoid;
}
