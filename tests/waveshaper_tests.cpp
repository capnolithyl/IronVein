#include "../Source/DSP/Waveshaper.h"
#include <cmath>
#include <iostream>

int main()
{
    for (int mode = 0; mode < 4; ++mode)
    {
        for (int i = -10000; i <= 10000; ++i)
        {
            const auto x = static_cast<float> (i) / 200.0f;
            const auto output = ironvein::shapeSample (x, mode);
            if (!std::isfinite (output) || output < -1.00001f || output > 1.00001f)
                return 1;
        }
        if (std::abs (ironvein::shapeSample (0.0f, mode)) > 1.0e-6f)
            return 2;
    }
    if (ironvein::shapeSample (2.0f, 1) != 1.0f
        || ironvein::shapeSample (-2.0f, 1) != -1.0f
        || ironvein::shapeSample (0.5f, 2) == -ironvein::shapeSample (-0.5f, 2))
        return 3;
    std::cout << "All four waveshaper modes passed.\n";
    return 0;
}
