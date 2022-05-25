#include "RandomGenerator.h"

namespace Beryll
{
    std::random_device RandomGenerator::rd;
    std::mt19937_64 RandomGenerator::re = std::mt19937_64(rd());
}