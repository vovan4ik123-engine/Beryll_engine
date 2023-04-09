#pragma once

#include "CppHeaders.h"

namespace Beryll
{
    class RandomGenerator
    {
    public:
        // return [0.0f ... 1.0f]
        static float getFloat()
        {
            return static_cast<float>(intDist(re)) / static_cast<float>(std::numeric_limits<int>::max());
        }

        // return [0 ... max]
        static int getInt(int max)
        {
            if(max < 1)
                return 0;

            if(max == std::numeric_limits<int>::max())
                return intDist(re);

            return intDist(re) % (max + 1);
        }

    private:
        // C++ 11 random generation way
        static std::mt19937_64 re; // random engine
        static std::uniform_int_distribution<int> intDist;
    };
}
