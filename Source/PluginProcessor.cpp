#include "PluginProcessor.h"
#include "PluginEditor.h"

IronveinAudioProcessor::IronveinAudioProcessor()
    : AudioProcessor (BusesProperties()
          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "IRONVEIN_STATE", createParameterLayout())
{
    rack.addModule (std::make_unique<ironvein::DistortionModule> (parameters));
    auto cab = std::make_unique<ironvein::CabinetModule> (parameters);
    cabinet = cab.get();
    rack.addModule (std::move (cab));
    parameters.state.setProperty ("preset.index", 0, nullptr);
}

juce::AudioProcessorValueTreeState::ParameterLayout IronveinAudioProcessor::createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<AudioParameterBool> (ParameterID {"dist.enabled", 1}, "Distortion Enabled", true));
    layout.add (std::make_unique<AudioParameterChoice> (ParameterID {"dist.mode", 1}, "Metal Voicing",
                StringArray {"Raw Black Metal", "Swedish Chainsaw", "Old-School Heavy", "Modern Death Metal", "Modern Metalcore"}, 0));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.drive", 1}, "Drive",
                NormalisableRange<float> (0.0f, 48.0f, 0.1f), 14.0f, AudioParameterFloatAttributes().withLabel ("dB")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.tight", 1}, "Tight (High Pass)",
                NormalisableRange<float> (35.0f, 350.0f, 1.0f, 0.5f), 205.0f, AudioParameterFloatAttributes().withLabel ("Hz")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.tone", 1}, "Tone (Low Pass)",
                NormalisableRange<float> (1800.0f, 16000.0f, 1.0f, 0.5f), 10700.0f, AudioParameterFloatAttributes().withLabel ("Hz")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.mix", 1}, "Mix",
                NormalisableRange<float> (0.0f, 100.0f, 1.0f), 100.0f, AudioParameterFloatAttributes().withLabel ("%")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.output", 1}, "Output",
                NormalisableRange<float> (-30.0f, 12.0f, 0.1f), -13.0f, AudioParameterFloatAttributes().withLabel ("dB")));
    layout.add (std::make_unique<AudioParameterBool> (ParameterID {"cab.enabled", 1}, "Cabinet Enabled", true));
    layout.add (std::make_unique<AudioParameterChoice> (ParameterID {"cab.ir", 1}, "Cabinet Response",
                StringArray {"1x12 Open", "2x12 Closed", "4x12 Straight", "4x12 Slant", "Custom WAV"}, 3));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"cab.level", 1}, "Cabinet Level",
                NormalisableRange<float> (-24.0f, 12.0f, 0.1f), -5.0f, AudioParameterFloatAttributes().withLabel ("dB")));

    // Three friendly macro controls, additive to the advanced settings.
    // Center=0, factory presets reset all three to zero.
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"macro.gain", 1}, "More / Less Gain",
                NormalisableRange<float> (-100.0f, 100.0f, 1.0f), 0.0f, AudioParameterFloatAttributes().withLabel ("%")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"macro.tight", 1}, "Loose / Tight",
                NormalisableRange<float> (-100.0f, 100.0f, 1.0f), 0.0f, AudioParameterFloatAttributes().withLabel ("%")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"macro.bright", 1}, "Dark / Bright",
                NormalisableRange<float> (-100.0f, 100.0f, 1.0f), 0.0f, AudioParameterFloatAttributes().withLabel ("%")));
    return layout;
}

void IronveinAudioProcessor::setRealParameter (const juce::String& id, float value)
{
    if (auto* param = parameters.getParameter (id))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost (param->convertTo0to1 (value));
        param->endChangeGesture();
    }
}

void IronveinAudioProcessor::applyFactoryPreset (int index)
{
    if (index < 0 || index >= static_cast<int> (ironvein::factoryPresets.size()))
        return;
    const auto& p = ironvein::factoryPresets[static_cast<size_t> (index)];
    setRealParameter ("dist.enabled", 1.0f);
    setRealParameter ("dist.mode", static_cast<float> (p.engine));
    setRealParameter ("dist.drive", p.driveDb);
    setRealParameter ("dist.tight", p.tightHz);
    setRealParameter ("dist.tone", p.toneHz);
    setRealParameter ("dist.mix", 100.0f);
    setRealParameter ("dist.output", p.outputDb);
    setRealParameter ("cab.enabled", 1.0f);
    setRealParameter ("cab.ir", static_cast<float> (p.cabinet));
    setRealParameter ("cab.level", p.cabinetDb);
    setRealParameter ("macro.gain", 0.0f);
    setRealParameter ("macro.tight", 0.0f);
    setRealParameter ("macro.bright", 0.0f);
    parameters.state.setProperty ("preset.index", index, nullptr);
}

int IronveinAudioProcessor::getSelectedFactoryPreset() const noexcept
{
    const auto v = parameters.state.getProperty ("preset.index", -1);
    return static_cast<int> (v);
}

bool IronveinAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    return (input == juce::AudioChannelSet::mono() || input == juce::AudioChannelSet::stereo())
           && layouts.getMainOutputChannelSet() == input;
}
void IronveinAudioProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    rack.prepare (sampleRate, maximumExpectedSamplesPerBlock, getTotalNumInputChannels());
    setLatencySamples (rack.getLatencySamples());
}
void IronveinAudioProcessor::releaseResources() { rack.reset(); }
void IronveinAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());
    rack.process (buffer);
}
juce::AudioProcessorEditor* IronveinAudioProcessor::createEditor()
{
    return new IronveinAudioProcessorEditor (*this);
}
bool IronveinAudioProcessor::loadCustomCabinetIR (const juce::File& file)
{
    return cabinet != nullptr && cabinet->loadCustomIR (file);
}
juce::String IronveinAudioProcessor::getCustomCabinetName() const
{
    const auto stored = parameters.state.getProperty ("cab.customIrPath").toString();
    return stored.isNotEmpty() ? juce::File (stored).getFileName() : "No custom WAV loaded";
}
void IronveinAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}
void IronveinAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType().toString()))
        {
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
            if (cabinet != nullptr)
                cabinet->requestReload();
        }
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new IronveinAudioProcessor(); }
