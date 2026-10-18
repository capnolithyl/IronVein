#pragma once
#include <algorithm>
#include <cmath>

namespace ironvein
{
// Five deliberately different nonlinearities. Tonal character additionally
// comes from the independent pre- and post-filter profiles in DistortionModule.
inline float shapeSample (float x, int mode) noexcept
{
    switch (mode)
    {
        case 0: // Raw Black: brittle, squared-off clipping
            return std::clamp (1.25f * x, -0.92f, 0.92f);
        case 1: // Swedish Chainsaw: severely saturated, rich upper harmonics
            return std::tanh (3.6f * x + 0.5f * std::sin (2.4f * x));
        case 2: // Old-school: dynamic, soft asymmetry
            return x >= 0.0f ? std::tanh (0.94f * x) : std::tanh (0.73f * x);
        case 3: // Death Metal: cascaded gain stages
            return std::tanh (2.1f * std::tanh (1.25f * x));
        case 4: // Modern Metalcore: tight, dense cascaded stages
            return std::tanh (2.85f * std::tanh (2.0f * x));
        default:
            return std::tanh (x);
    }
}
}
