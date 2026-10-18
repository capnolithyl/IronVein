#pragma once
#include "EffectModule.h"
#include "AmpVoices.h"
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
    std::atomic<float>* macroGain = nullptr;
    std::atomic<float>* macroTight = nullptr;
    std::atomic<float>* macroBright = nullptr;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
    juce::AudioBuffer<float> delayedDry;
    std::vector<std::vector<float>> dryHistory;
    std::array<amp::State, 2> engineState;
    juce::SmoothedValue<float> driveGain, outputGain, mix, moduleOn;
    double currentSampleRate = 44100.0;
    int maxBlock = 512;
    int channelCount = 2;
    int latencySamples = 0;
    int dryWriteIndex = 0;
    int lastMode = -1;
};
}
