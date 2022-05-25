#pragma once

#include "CppHeaders.h"

namespace Beryll
{
    class RandomGenerator
    {
    public:

        static int getQualityInt(int first, int last)
        {
            std::uniform_int_distribution<int> dist(first, last);
            return dist(re);
        }

        static int getFastInt(int first, int last)
        {
            return (std::rand() % (last - first)) + first;
        }

        static float getQualityFloat(float first, float last)
        {
            std::uniform_real_distribution<float> dist(first, last);
            return dist(re);
        }

        static float getFastFloat(float first, float last)
        {
            return ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (last - first)) + first;
        }

    private:
        // C++ 11 random generation way
        static std::random_device rd;
        static std::mt19937_64 re; // random engine
    };
}
