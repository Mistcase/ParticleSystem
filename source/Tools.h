#pragma once

#include <cstdlib>

namespace tools
{
    inline float RandomFloat(float valueMin, float valueMax)
    {
        return valueMin + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (valueMax - valueMin)));
    }

} // namespace tools