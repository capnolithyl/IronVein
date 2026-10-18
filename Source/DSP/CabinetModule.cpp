#include "CabinetModule.h"
#include <BinaryData.h>

namespace ironvein
{
namespace
{
struct IRResource { const void* data; size_t bytes; };
IRResource bundled (int index)
{
    switch (index)
    {
        case 0: return {BinaryData::Cab_Open_1x12_wav, static_cast<size_t> (BinaryData::Cab_Open_1x12_wavSize)};
        case 1: return {BinaryData::Cab_Closed_2x12_wav, static_cast<size_t> (BinaryData::Cab_Closed_2x12_wavSize)};
        case 2: return {BinaryData::Cab_Straight_4x12_wav, static_cast<size_t> (BinaryData::Cab_Straight_4x12_wavSize)};
        case 3: return {BinaryData::Cab_Slant_4x12_wav, static_cast<size_t> (BinaryData::Cab_Slant_4x12_wavSize)};
        default: return {BinaryData::Cab_Straight_4x12_wav, static_cast<size_t> (BinaryData::Cab_Straight_4x12_wavSize)};
    }
}
}

CabinetModule::CabinetModule (juce::AudioProcessorValueTreeState& params)
    : state (params),
      enabledParam (state.getRawParameterValue ("cab.enabled")),
      irParam (state.getRawParameterValue ("cab.ir")),
      levelParam (state.getRawParameterValue ("cab.level"))
{
    jassert (enabledParam && irParam && levelParam);
}

CabinetModule::~CabinetModule()
{
    cancelPendingUpdate();
}

void CabinetModule::prepare (double sampleRate, int maximumBlockSize, int channels)
{
    maxBlock = juce::jmax (1, maximumBlockSize);
    convolution.prepare ({sampleRate, static_cast<juce::uint32> (maxBlock),
                          static_cast<juce::uint32> (juce::jlimit (1, 2, channels))});
    outputGain.reset (sampleRate, 0.025);
    outputGain.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (levelParam->load()));
    loadedSelection.store (-1);
    // The convolution engine builds responses asynchronously inside JUCE.
    loadSelection (juce::jlimit (0, 4, juce::roundToInt (irParam->load())));
}

void CabinetModule::reset()
{
    convolution.reset();
}

bool CabinetModule::loadCustomIR (const juce::File& file)
{
    if (! file.existsAsFile() || ! file.hasFileExtension (".wav") || file.getSize() > 32 * 1024 * 1024)
        return false;

    state.state.setProperty ("cab.customIrPath", file.getFullPathName(), nullptr);
    requestReload();
    return true;
}

void CabinetModule::requestReload()
{
    loadedSelection.store (-1);
    triggerAsyncUpdate();
}

void CabinetModule::handleAsyncUpdate()
{
    loadSelection (juce::jlimit (0, 4, juce::roundToInt (irParam->load())));
}

void CabinetModule::loadSelection (int index)
{
    const auto requestedIndex = index;
    if (index == 4)
    {
        auto path = state.state.getProperty ("cab.customIrPath").toString();
        const juce::File file (path);
        if (file.existsAsFile() && file.hasFileExtension (".wav") && file.getSize() <= 32 * 1024 * 1024)
        {
            convolution.loadImpulseResponse (file, juce::dsp::Convolution::Stereo::no,
                                             juce::dsp::Convolution::Trim::yes, 0,
                                             juce::dsp::Convolution::Normalise::yes);
            loadedSelection.store (index);
            return;
        }
        // A missing user file must not crash old saved sessions.
        index = 2;
    }

    const auto asset = bundled (index);
    convolution.loadImpulseResponse (asset.data, asset.bytes,
                                     juce::dsp::Convolution::Stereo::no,
                                     juce::dsp::Convolution::Trim::yes, 0,
                                     juce::dsp::Convolution::Normalise::yes);
    // If a saved custom path has disappeared, use a bundled cabinet without
    // continuously retrying the missing file on every audio callback.
    loadedSelection.store (requestedIndex);
}

void CabinetModule::process (juce::AudioBuffer<float>& audio)
{
    const auto wanted = juce::jlimit (0, 4, juce::roundToInt (irParam->load()));
    if (wanted != loadedSelection.load())
    {
        // Never load/parse WAV files on the real-time audio thread.
        // AsyncUpdater coalesces host automation changes on the message thread.
        loadedSelection.store (-2);
        triggerAsyncUpdate();
    }
    if (enabledParam->load() < 0.5f)
        return;

    outputGain.setTargetValue (juce::Decibels::decibelsToGain (levelParam->load()));
    for (int offset = 0; offset < audio.getNumSamples(); offset += maxBlock)
    {
        const auto n = juce::jmin (maxBlock, audio.getNumSamples() - offset);
        auto block = juce::dsp::AudioBlock<float> (audio)
                          .getSubBlock (static_cast<size_t> (offset), static_cast<size_t> (n));
        convolution.process (juce::dsp::ProcessContextReplacing<float> (block));
        for (int i = 0; i < n; ++i)
        {
            const auto gain = outputGain.getNextValue();
            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
                audio.setSample (ch, offset + i, audio.getSample (ch, offset + i) * gain);
        }
    }
}
}
