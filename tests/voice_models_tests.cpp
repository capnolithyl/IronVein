#include "../Source/DSP/AmpVoices.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

int main()
{
    constexpr float sr = 48000.0f * 4.0f;
    constexpr int length = 4096;
    std::array<std::array<float, length>, 5> responses {};
    for (int mode = 0; mode < 5; ++mode)
    {
        ironvein::amp::State state;
        const auto coeff = ironvein::amp::makeCoefficients (mode, 90.0f, 12000.0f, sr);
        float energy = 0.0f;
        for (int n = 0; n < length; ++n)
        {
            const float t = static_cast<float> (n) / sr;
            const float input = (0.2f * std::sin (ironvein::amp::pi2 * 110.0f * t)
                + 0.14f * std::sin (ironvein::amp::pi2 * 740.0f * t)
                + 0.085f * std::sin (ironvein::amp::pi2 * 3100.0f * t));
            const float y = ironvein::amp::process (mode, input * 4.0f, state, coeff);
            if (! std::isfinite (y) || std::abs (y) > 8.0f)
                return 1;
            responses[static_cast<size_t> (mode)][static_cast<size_t> (n)] = y;
            energy += y * y;
        }
        if (energy < 0.001f)
            return 2;
    }

    // Compare waveform shapes after matching overall gain to verify that the
    // topology changes cannot be explained away as a volume adjustment.
    for (int a = 0; a < 5; ++a)
    {
        for (int b = a + 1; b < 5; ++b)
        {
            double aa = 0, bb = 0, ab = 0;
            for (int i = 512; i < length; ++i)
            {
                const auto x = responses[a][i], y = responses[b][i];
                aa += x * x; bb += y * y; ab += x * y;
            }
            const double correlation = ab / std::sqrt (aa * bb);
            std::cout << "Voice " << a << " vs " << b << " correlation: " << correlation << '\n';
            if (std::abs (correlation) > 0.9995)
                return 3;
        }
    }
    std::cout << "Five distinct amp structures produce nontrivial, finite audio.\n";
    return 0;
}
