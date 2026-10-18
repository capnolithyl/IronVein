#pragma once
#include "EffectModule.h"
#include <memory>
#include <vector>

namespace ironvein
{
// Processes DSP modules in order. Runtime mutation is intentionally not yet supported.
class EffectRack final
{
public:
    void addModule (std::unique_ptr<EffectModule> module)
    {
        modules.push_back (std::move (module));
    }

    void prepare (double sampleRate, int maxBlock, int channels)
    {
        for (auto& module : modules)
            module->prepare (sampleRate, maxBlock, channels);
    }

    void reset()
    {
        for (auto& module : modules)
            module->reset();
    }

    void process (juce::AudioBuffer<float>& audio)
    {
        for (auto& module : modules)
            module->process (audio);
    }

    int getLatencySamples() const noexcept
    {
        int sum = 0;
        for (const auto& module : modules)
            sum += module->getLatencySamples();
        return sum;
    }

private:
    std::vector<std::unique_ptr<EffectModule>> modules;
};
}
