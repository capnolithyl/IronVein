#include "../Source/DSP/Waveshaper.h"
#include <algorithm>
#include <cmath>
#include <iostream>

int main()
{
    float signatures[5] {};
    for (int mode = 0; mode < 5; ++mode)
    {
        float signature = 0.0f;
        for (int i = -10000; i <= 10000; ++i)
        {
            const auto x = static_cast<float> (i) / 200.0f;
            const auto output = ironvein::shapeSample (x, mode);
            if (!std::isfinite (output) || output < -1.00001f || output > 1.00001f)
                return 1;
            if (i >= -180 && i <= 180)
                signature += std::abs (output);
        }
        signatures[mode] = signature;
        if (std::abs (ironvein::shapeSample (0.0f, mode)) > 1.0e-6f)
            return 2;
    }
    // Ensure character selections do not all resolve to the same transfer function.
    for (int i = 0; i < 5; ++i)
        for (int j = i + 1; j < 5; ++j)
            if (std::abs (signatures[i] - signatures[j]) < 0.1f)
                return 3;
    std::cout << "Five finite, distinct waveshaper modes passed.\n";
    return 0;
}
