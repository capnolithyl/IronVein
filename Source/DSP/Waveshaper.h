#pragma once
#include <algorithm>
#include <cmath>

namespace ironvein
{
// Pure and host-independent: unit-tested without JUCE.
inline float shapeSample (float x, int mode) noexcept
{
    switch (mode)
    {
        case 0: return std::tanh (0.85f * x); // Warm
        case 1: return std::clamp (x, -1.0f, 1.0f); // Hard
        case 2: return x >= 0.0f ? std::tanh (1.8f * x) : std::tanh (0.95f * x); // Razor
        case 3: return std::tanh (2.8f * x); // Obliterate
        default: return x;
    }
}
}
