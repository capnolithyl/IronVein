#include "DistortionModule.h"
#include <cmath>

namespace ironvein
{
namespace
{
struct Voicing
{
    float highPassHz;
    float preBassHz;
    float preMidHz;
    float preBassAmount;
    float midAmount;
    float trebleAmount;
    float postHighCutHz;
    float postMidAmount;
    float gainTrim;
};

constexpr Voicing voicings[] =
{
    // Raw black: fizzy treble emphasis, intentionally light bass.
    { 300.0f, 420.0f, 2600.0f, -0.65f, 0.10f, 0.68f, 11800.0f, -0.18f, 0.54f },
    // Swedish chainsaw: biting high mids and nasty dual-band emphasis.
    {  90.0f, 490.0f, 2800.0f, -0.26f, 1.65f, 0.42f, 8500.0f, 0.55f, 0.43f },
    // Old-school: open, forward low mids and less fizz.
    {  65.0f, 550.0f, 2000.0f, 0.68f, 0.30f, -0.10f, 6200.0f, 0.10f, 0.77f },
    // Death: focused attack, dense gain, controlled low end.
    { 135.0f, 500.0f, 3200.0f, -0.15f, 0.85f, 0.24f, 7800.0f, 0.28f, 0.51f },
    // Metalcore: tight pre-high-pass and bright attack, smoother after cab.
    { 175.0f, 630.0f, 3900.0f, -0.35f, 0.52f, 0.80f, 7200.0f, 0.12f, 0.43f }
};

float coeff (double hz, double sr) noexcept
{
    return static_cast<float> (1.0 - std::exp (-juce::MathConstants<double>::twoPi * hz / sr));
}
}

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
    oversampling = std::make_unique<juce::dsp::Oversampling<float>> (
        static_cast<size_t> (channelCount), 2,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true);
    oversampling->initProcessing (static_cast<size_t> (maxBlock));
    latencySamples = juce::roundToInt (oversampling->getLatencyInSamples());

    delayedDry.setSize (channelCount, maxBlock, false, false, true);
    dryHistory.assign (static_cast<size_t> (channelCount),
                       std::vector<float> (static_cast<size_t> (latencySamples), 0.0f));
    dryWriteIndex = 0;

    driveGain.reset (sampleRate * oversamplingFactor, 0.02);
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
    preBassState.fill (0.0f);
    preMidState.fill (0.0f);
    postMidState.fill (0.0f);
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

    const auto mode = juce::jlimit (0, 4, juce::roundToInt (modeParam->load()));
    const auto& voice = voicings[mode];
    const auto sampleRateOS = currentSampleRate * oversamplingFactor;
    const auto hpCoefficient = coeff (juce::jmax (voice.highPassHz, tightParam->load()), sampleRateOS);
    const auto bassCoefficient = coeff (voice.preBassHz, sampleRateOS);
    const auto midCoefficient = coeff (voice.preMidHz, sampleRateOS);
    const auto postCoefficient = coeff (1400.0, sampleRateOS);
    const auto lpCoefficient = coeff (juce::jmin (voice.postHighCutHz, toneParam->load()), sampleRateOS);
    const auto dcPole = static_cast<float> (std::exp (-juce::MathConstants<double>::twoPi * 15.0 / sampleRateOS));

    driveGain.setTargetValue (juce::Decibels::decibelsToGain (driveParam->load()));
    outputGain.setTargetValue (juce::Decibels::decibelsToGain (outputParam->load()));
    mix.setTargetValue (mixParam->load() * 0.01f);
    moduleOn.setTargetValue (enabledParam->load() > 0.5f ? 1.0f : 0.0f);

    for (int offset = 0; offset < audio.getNumSamples(); offset += maxBlock)
    {
        const auto numSamples = juce::jmin (maxBlock, audio.getNumSamples() - offset);
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

        for (int n = 0; n < upsampledSamples; ++n)
        {
            const auto inputGain = driveGain.getNextValue();
            for (int ch = 0; ch < channels; ++ch)
            {
                const auto i = static_cast<size_t> (ch);
                const auto x = upsampled.getSample (i, static_cast<size_t> (n));
                auto& hp = highPassLowState[i];
                hp += hpCoefficient * (x - hp);
                const auto highPassed = x - hp;

                auto& bassLow = preBassState[i];
                auto& midLow = preMidState[i];
                bassLow += bassCoefficient * (highPassed - bassLow);
                midLow += midCoefficient * (highPassed - midLow);
                const auto midBand = midLow - bassLow;
                const auto treble = highPassed - midLow;

                const auto preEmphasis = highPassed
                                       + voice.preBassAmount * bassLow
                                       + voice.midAmount * midBand
                                       + voice.trebleAmount * treble;

                auto shaped = shapeSample (inputGain * preEmphasis, mode);
                auto& dcIn = dcInputState[i];
                auto& dcOut = dcOutputState[i];
                const auto dcFiltered = shaped - dcIn + dcPole * dcOut;
                dcIn = shaped;
                dcOut = dcFiltered;

                auto& postMid = postMidState[i];
                postMid += postCoefficient * (dcFiltered - postMid);
                const auto boosted = dcFiltered + voice.postMidAmount * (dcFiltered - postMid);

                auto& lp = lowPassState[i];
                lp += lpCoefficient * (boosted - lp);
                upsampled.setSample (i, static_cast<size_t> (n), voice.gainTrim * lp);
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
