#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "Beryll/Core/Log.h"

namespace Beryll
{
    // Any parallel execution should be done by this class.
    // Dont create any threads outside this class !!!
    // std::function<...> will called from many threads
    // If it access common memory that can be accessed in other thread sync this memory with mutex
    class AsyncRun
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

            if(numberElements <= 50 || m_numThreads == 1)
            {
                // run on main thread
                func(v, 0, numberElements);
            }
            else
            {
                m_futuresVoid.clear();

                int oneChunkSize = numberElements / m_numThreads;
                oneChunkSize++; // make sure (oneChunkSize * m_numThreads) > numberElements

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

        }

        static const uint32_t m_numThreads; // all available threads on device -1
        static std::vector<std::future<void>> m_futuresVoid;
    };
}

