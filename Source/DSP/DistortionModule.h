#pragma once
#include "EffectModule.h"
#include "Waveshaper.h"
#include <array>
#include <memory>
#include <vector>

namespace ironvein
{
class DistortionModule final : public EffectModule
{
public:
    explicit DistortionModule (juce::AudioProcessorValueTreeState& state);
    void prepare (double sampleRate, int maximumBlockSize, int channels) override;
    void reset() override;
    void process (juce::AudioBuffer<float>& audio) override;
    int getLatencySamples() const noexcept override { return latencySamples; }

    static constexpr int oversamplingFactor = 4;

private:
    std::atomic<float>* enabledParam = nullptr;
    std::atomic<float>* modeParam = nullptr;
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* tightParam = nullptr;
    std::atomic<float>* toneParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* outputParam = nullptr;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
    juce::AudioBuffer<float> delayedDry;
    std::vector<std::vector<float>> dryHistory;
    std::array<float, 2> highPassLowState {};
    std::array<float, 2> preBassState {};
    std::array<float, 2> preMidState {};
    std::array<float, 2> postMidState {};
    std::array<float, 2> lowPassState {};
    std::array<float, 2> dcInputState {};
    std::array<float, 2> dcOutputState {};
    juce::SmoothedValue<float> driveGain;
    juce::SmoothedValue<float> outputGain;
    juce::SmoothedValue<float> mix;
    juce::SmoothedValue<float> moduleOn;
    double currentSampleRate = 44100.0;
    int maxBlock = 512;
    int channelCount = 2;
    int latencySamples = 0;
    int dryWriteIndex = 0;
};
}
