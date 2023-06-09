#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "beryll/core/Log.h"

namespace Beryll
{
    // Any parallel execution should be done by this class.
    // std::function<...> will called from many threads.
    // If it access common memory that can be accessed in other thread you must sync this memory.
    class AsyncRun final
    {
    public:
        AsyncRun() = delete;
        ~AsyncRun() = delete;

        uint32_t getThreadsNumber()
        {
            return m_numThreads;
        }

        template<typename T>
        static void Run(std::vector<T>& v, std::function<void(std::vector<T>&, int, int)> func)
        {
            const int numberElements = v.size();

            //BR_INFO("m_numThreads: %d, numberElements: %d", m_numThreads, numberElements);

            if(numberElements == 1 || m_numThreads <= 1)
            {
                // Run on main thread and exit.
                func(v, 0, numberElements);
                BR_WARN("%s", "AsyncRun::Run() was executed on main thread. Consider dont call AsyncRun::Run()");
                return;
            }

            // Run in std::async()
            m_futuresVoid.clear();

            float chunkFloat = float(numberElements) / float(m_numThreads);
            int oneChunkSize = 0;
            if(chunkFloat > float(int(chunkFloat)))
                oneChunkSize = int(chunkFloat) + 1;
            else
                oneChunkSize = int(chunkFloat);

            //BR_INFO("chunkFloat: %f, oneChunkSize: %d", chunkFloat, oneChunkSize);

            for(int i = 0; i < numberElements; i += oneChunkSize)
            {
                int chunkEnd = std::min(i + oneChunkSize, numberElements);

                // without std::cref(v)/std::ref(v) std::async() will COPY all parameters !!!
                m_futuresVoid.emplace_back(std::async(std::launch::async, func, std::ref(v), i, chunkEnd));
            }

            // wait all threads
            for(const std::future<void>& ft : m_futuresVoid)
            {
                ft.wait();
            }
        }

        static const uint32_t m_numThreads; // all available threads on device -1
        static std::vector<std::future<void>> m_futuresVoid;
    };
}

