#pragma once

#include "CppHeaders.h"

namespace Beryll
{
    class RandomGenerator
    {
    public:

        static int getQualityInt(const unsigned int first, const unsigned int last)
        {
            std::uniform_int_distribution<int> dist(first, last);
            return dist(re);
        }

        static int getFastInt(const unsigned int first, const unsigned int last)
        {
            return (std::rand() % (last - first)) + first;
        }

        static float getQualityFloat(float first, float last)
        {
            std::uniform_real_distribution<float> dist(first, last);
            return dist(re);
        }

        // return 0.0f ... 1.0f
        static float getFastFloat()
        {
            return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        }

    private:
        // C++ 11 random generation way
        static std::random_device rd;
        static std::mt19937_64 re; // random engine
    };
}
