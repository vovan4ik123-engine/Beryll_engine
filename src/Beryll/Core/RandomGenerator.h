#pragma once

#include "CppHeaders.h"

namespace Beryll
{
    class RandomGenerator
    {
    public:
        // return 0.0f ... 1.0f
        static float getFloat()
        {
            return static_cast<float>(intDist(re)) / static_cast<float>(std::numeric_limits<int>::max());
        }

        // return 0 ... max
        static int getInt(int max)
        {
            return getFloat() * static_cast<float>(max);
        }

    private:
        // C++ 11 random generation way
        static std::mt19937_64 re; // random engine
        static std::uniform_int_distribution<int> intDist;
    };
}
