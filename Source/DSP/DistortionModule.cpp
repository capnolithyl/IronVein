#include "DistortionModule.h"
#include <cmath>

namespace ironvein
{
DistortionModule::DistortionModule (juce::AudioProcessorValueTreeState& state)
    : enabledParam (state.getRawParameterValue ("dist.enabled")),
      modeParam (state.getRawParameterValue ("dist.mode")),
      driveParam (state.getRawParameterValue ("dist.drive")),
      tightParam (state.getRawParameterValue ("dist.tight")),
      toneParam (state.getRawParameterValue ("dist.tone")),
      mixParam (state.getRawParameterValue ("dist.mix")),
      outputParam (state.getRawParameterValue ("dist.output")),
      macroGain (state.getRawParameterValue ("macro.gain")),
      macroTight (state.getRawParameterValue ("macro.tight")),
      macroBright (state.getRawParameterValue ("macro.bright"))
{
    jassert (enabledParam && modeParam && driveParam && tightParam && toneParam &&
             mixParam && outputParam && macroGain && macroTight && macroBright);
}

void DistortionModule::prepare (double sampleRate, int maximumBlockSize, int channels)
{
    currentSampleRate = sampleRate;
    maxBlock = juce::jmax (1, maximumBlockSize);
    channelCount = juce::jlimit (1, 2, channels);
    oversampling = std::make_unique<juce::dsp::Oversampling<float>> (
        static_cast<size_t> (channelCount), 2,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true);
    oversampling->initProcessing (static_cast<size_t> (maxBlock));
    latencySamples = juce::roundToInt (oversampling->getLatencyInSamples());

    delayedDry.setSize (channelCount, maxBlock, false, false, true);
    dryHistory.assign (static_cast<size_t> (channelCount),
                       std::vector<float> (static_cast<size_t> (latencySamples), 0.0f));
    driveGain.reset (sampleRate * oversamplingFactor, 0.025);
    driveGain.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (driveParam->load()));
    outputGain.reset (sampleRate, 0.035);
    outputGain.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (outputParam->load()));
    mix.reset (sampleRate, 0.02);
    mix.setCurrentAndTargetValue (mixParam->load() * 0.01f);
    moduleOn.reset (sampleRate, 0.02);
    moduleOn.setCurrentAndTargetValue (enabledParam->load() > 0.5f ? 1.0f : 0.0f);
    reset();
}

void DistortionModule::reset()
{
    if (oversampling != nullptr)
        oversampling->reset();
    for (auto& channel : dryHistory)
        std::fill (channel.begin(), channel.end(), 0.0f);
    for (auto& state : engineState)
        state = {};
    dryWriteIndex = 0;
    lastMode = -1;
}

void DistortionModule::process (juce::AudioBuffer<float>& audio)
{
    if (oversampling == nullptr || audio.getNumSamples() == 0)
        return;
    const auto channels = juce::jmin (audio.getNumChannels(), channelCount);
    if (channels == 0)
        return;

    const auto mode = juce::jlimit (0, 4, juce::roundToInt (modeParam->load()));
    // Coefficients are computed once per audio block, not per sample.
    const float gainOffset = 0.12f * macroGain->load(); // +/-12 dB
    const float tightOffset = 0.85f * macroTight->load(); // +/-85 Hz
    const float bright = macroBright->load();
    const float highCut = juce::jlimit (1800.0f, 16000.0f,
                                        toneParam->load() * std::pow (2.0f, bright / 110.0f));
    const auto c = amp::makeCoefficients (
        mode, juce::jlimit (35.0f, 350.0f, tightParam->load() + tightOffset),
        highCut, static_cast<float> (currentSampleRate * oversamplingFactor));

    if (mode != lastMode)
    {
        // Avoid dragging old filter/envelope states between completely different circuits.
        for (auto& state : engineState)
            state = {};
        lastMode = mode;
    }

    driveGain.setTargetValue (juce::Decibels::decibelsToGain (
        juce::jlimit (0.0f, 48.0f, driveParam->load() + gainOffset)));
    outputGain.setTargetValue (juce::Decibels::decibelsToGain (outputParam->load()));
    mix.setTargetValue (mixParam->load() * 0.01f);
    moduleOn.setTargetValue (enabledParam->load() > 0.5f ? 1.0f : 0.0f);

    for (int offset = 0; offset < audio.getNumSamples(); offset += maxBlock)
    {
        const auto count = juce::jmin (maxBlock, audio.getNumSamples() - offset);
        for (int n = 0; n < count; ++n)
        {
            for (int ch = 0; ch < channels; ++ch)
            {
                const auto original = audio.getSample (ch, offset + n);
                auto delayed = original;
                if (latencySamples > 0)
                {
                    delayed = dryHistory[static_cast<size_t> (ch)][static_cast<size_t> (dryWriteIndex)];
                    dryHistory[static_cast<size_t> (ch)][static_cast<size_t> (dryWriteIndex)] = original;
                }
                delayedDry.setSample (ch, n, delayed);
            }
            if (latencySamples > 0 && ++dryWriteIndex >= latencySamples)
                dryWriteIndex = 0;
        }

        auto wet = juce::dsp::AudioBlock<float> (audio)
                       .getSubBlock (static_cast<size_t> (offset), static_cast<size_t> (count));
        auto upsampled = oversampling->processSamplesUp (wet);
        for (size_t n = 0; n < upsampled.getNumSamples(); ++n)
        {
            const auto gain = driveGain.getNextValue();
            for (int ch = 0; ch < channels; ++ch)
            {
                const auto i = static_cast<size_t> (ch);
                const float x = upsampled.getSample (i, n);
                const auto result = amp::process (mode, x * gain, engineState[i], c);
                upsampled.setSample (i, n, result);
            }
        }
        oversampling->processSamplesDown (wet);

        for (int n = 0; n < count; ++n)
        {
            const auto wetAmount = mix.getNextValue();
            const auto gain = outputGain.getNextValue();
            const auto active = moduleOn.getNextValue();
            for (int ch = 0; ch < channels; ++ch)
            {
                const auto dry = delayedDry.getSample (ch, n);
                const auto distorted = audio.getSample (ch, offset + n);
                const auto blended = ((1.0f - wetAmount) * dry + wetAmount * distorted) * gain;
                audio.setSample (ch, offset + n, dry + active * (blended - dry));
            }
        }
    }
}
}
