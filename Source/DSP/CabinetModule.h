#pragma once
#include "EffectModule.h"
#include <atomic>

namespace ironvein
{
class CabinetModule final : public EffectModule, private juce::AsyncUpdater
{
public:
    explicit CabinetModule (juce::AudioProcessorValueTreeState& state);
    ~CabinetModule() override;

    void prepare (double sampleRate, int maximumBlockSize, int channels) override;
    void reset() override;
    void process (juce::AudioBuffer<float>& audio) override;
    int getLatencySamples() const noexcept override { return 0; }

    // Call from the GUI/message thread, never from processBlock.
    bool loadCustomIR (const juce::File& file);
    void requestReload();

private:
    void handleAsyncUpdate() override;
    void loadSelection (int index);

    juce::AudioProcessorValueTreeState& state;
    std::atomic<float>* enabledParam = nullptr;
    std::atomic<float>* irParam = nullptr;
    std::atomic<float>* levelParam = nullptr;
    juce::dsp::Convolution convolution;
    juce::SmoothedValue<float> outputGain;
    std::atomic<int> loadedSelection { -1 };
    int maxBlock = 512;
};
}
