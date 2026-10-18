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
      outputParam (state.getRawParameterValue ("dist.output"))
{
    jassert (enabledParam && modeParam && driveParam && tightParam && toneParam
             && mixParam && outputParam);
}

void DistortionModule::prepare (double sampleRate, int maximumBlockSize, int channels)
{
    currentSampleRate = sampleRate;
    maxBlock = juce::jmax (1, maximumBlockSize);
    channelCount = juce::jlimit (1, 2, channels);

    // 2 is log2(4x). Integer latency permits sample-exact dry/wet delay alignment.
    oversampling = std::make_unique<juce::dsp::Oversampling<float>> (
        static_cast<size_t> (channelCount), 2,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true);
    oversampling->initProcessing (static_cast<size_t> (maxBlock));
    latencySamples = juce::roundToInt (oversampling->getLatencyInSamples());

    delayedDry.setSize (channelCount, maxBlock, false, false, true);
    dryHistory.assign (static_cast<size_t> (channelCount),
                       std::vector<float> (static_cast<size_t> (latencySamples), 0.0f));
    dryWriteIndex = 0;

    driveGain.reset (sampleRate * oversamplingFactor, 0.025);
    driveGain.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (driveParam->load()));
    outputGain.reset (sampleRate, 0.025);
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
    highPassLowState.fill (0.0f);
    lowPassState.fill (0.0f);
    dcInputState.fill (0.0f);
    dcOutputState.fill (0.0f);
    dryWriteIndex = 0;
}

void DistortionModule::process (juce::AudioBuffer<float>& audio)
{
    if (oversampling == nullptr)
        return;

    const auto channels = juce::jmin (audio.getNumChannels(), channelCount);
    if (channels <= 0 || audio.getNumSamples() == 0)
        return;

    const auto mode = juce::jlimit (0, 3, juce::roundToInt (modeParam->load()));
    const auto sampleRateOS = currentSampleRate * oversamplingFactor;
    const auto tightHz = static_cast<double> (tightParam->load());
    const auto toneHz = static_cast<double> (toneParam->load());
    const auto hpCoefficient = static_cast<float> (1.0 - std::exp (-juce::MathConstants<double>::twoPi * tightHz / sampleRateOS));
    const auto lpCoefficient = static_cast<float> (1.0 - std::exp (-juce::MathConstants<double>::twoPi * toneHz / sampleRateOS));
    const auto dcPole = static_cast<float> (std::exp (-juce::MathConstants<double>::twoPi * 15.0 / sampleRateOS));

    driveGain.setTargetValue (juce::Decibels::decibelsToGain (driveParam->load()));
    outputGain.setTargetValue (juce::Decibels::decibelsToGain (outputParam->load()));
    mix.setTargetValue (mixParam->load() * 0.01f);
    moduleOn.setTargetValue (enabledParam->load() > 0.5f ? 1.0f : 0.0f);

    for (int offset = 0; offset < audio.getNumSamples(); offset += maxBlock)
    {
        const auto numSamples = juce::jmin (maxBlock, audio.getNumSamples() - offset);

        // Delays the unprocessed signal by the same sample latency as oversampling.
        for (int n = 0; n < numSamples; ++n)
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

        auto wetBlock = juce::dsp::AudioBlock<float> (audio)
                            .getSubBlock (static_cast<size_t> (offset), static_cast<size_t> (numSamples));
        auto upsampled = oversampling->processSamplesUp (wetBlock);
        const auto upsampledSamples = static_cast<int> (upsampled.getNumSamples());

        // Advance drive once per oversampled frame so stereo channels track.
        for (int n = 0; n < upsampledSamples; ++n)
        {
            const auto gain = driveGain.getNextValue();
            for (int ch = 0; ch < channels; ++ch)
            {
                const auto x = upsampled.getSample (static_cast<size_t> (ch), static_cast<size_t> (n));
                auto& hp = highPassLowState[static_cast<size_t> (ch)];
                hp += hpCoefficient * (x - hp);
                auto distorted = shapeSample ((x - hp) * gain, mode);

                // Asymmetric clipping introduces DC; cancel it before tone filtering.
                auto& dcIn = dcInputState[static_cast<size_t> (ch)];
                auto& dcOut = dcOutputState[static_cast<size_t> (ch)];
                const auto dcFiltered = distorted - dcIn + dcPole * dcOut;
                dcIn = distorted;
                dcOut = dcFiltered;

                auto& lp = lowPassState[static_cast<size_t> (ch)];
                lp += lpCoefficient * (dcFiltered - lp);
                upsampled.setSample (static_cast<size_t> (ch), static_cast<size_t> (n), lp);
            }
        }

        oversampling->processSamplesDown (wetBlock);

        for (int n = 0; n < numSamples; ++n)
        {
            const auto wetAmount = mix.getNextValue();
            const auto gain = outputGain.getNextValue();
            const auto active = moduleOn.getNextValue();
            for (int ch = 0; ch < channels; ++ch)
            {
                const auto dry = delayedDry.getSample (ch, n);
                const auto wet = audio.getSample (ch, offset + n);
                const auto processed = ((1.0f - wetAmount) * dry + wetAmount * wet) * gain;
                audio.setSample (ch, offset + n, dry + active * (processed - dry));
            }
        }
    }
}
}
