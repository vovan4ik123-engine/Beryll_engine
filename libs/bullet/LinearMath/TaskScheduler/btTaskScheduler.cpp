#if BT_THREADSAFE

#include "LinearMath/btThreads.h"

#include <vector>
#include <future>
#include <thread>
#include <algorithm>
//#include <android/log.h>

class btTaskSchedulerForBeryll : public btITaskScheduler
{
public:
    btTaskSchedulerForBeryll()
    {
        // All available threads -1.
        m_numThreads = std::max((std::thread::hardware_concurrency() == 0 ? 0 : std::thread::hardware_concurrency() - 1u), 1u);

        m_futuresVoid.reserve(m_numThreads);
        m_futuresFloat.reserve(m_numThreads);
    }

    ~btTaskSchedulerForBeryll() override
    {
        m_futuresVoid.clear();
        m_futuresFloat.clear();
    }

    int getNumThreads() const override { return m_numThreads; }

    void parallelFor(int iBegin, int iEnd, int grainSize, const btIParallelForBody& body) override
    {
        //__android_log_print(ANDROID_LOG_DEBUG, "parallel For", "numberElements: %d", iEnd - iBegin);
        // Run on main thread.
        body.forLoop(iBegin, iEnd);
    }

    float parallelSum(int iBegin, int iEnd, int grainSize, const btIParallelSumBody& body) override
    {
        //__android_log_print(ANDROID_LOG_DEBUG, "parallel Sum", "numberElements: %d", iEnd - iBegin);
        // Run on main thread.
        return body.sumLoop(iBegin, iEnd);
    }

private:
    int m_numThreads = 1;
    std::vector<std::future<void>> m_futuresVoid;
    std::vector<std::future<float>> m_futuresFloat;
};

btITaskScheduler* btCreateTaskSchedulerForBeryll()
{
    btTaskSchedulerForBeryll* ts = new btTaskSchedulerForBeryll();
    return ts;
}

#endif // #if BT_THREADSAFE
