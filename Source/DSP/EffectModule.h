#pragma once
#include <JuceHeader.h>

namespace ironvein
{
class EffectModule
{
public:
    virtual ~EffectModule() = default;
    virtual void prepare (double sampleRate, int maximumBlockSize, int channels) = 0;
    virtual void reset() = 0;
    virtual void process (juce::AudioBuffer<float>& audio) = 0;
    virtual int getLatencySamples() const noexcept = 0;
};
}
