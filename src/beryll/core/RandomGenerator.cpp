#include "RandomGenerator.h"

namespace Beryll
{
    std::mt19937_64 RandomGenerator::re = std::mt19937_64(std::random_device()());
    std::uniform_int_distribution<int> RandomGenerator::intDist;
}