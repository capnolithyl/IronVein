#include "PluginProcessor.h"
#include "PluginEditor.h"

IronveinAudioProcessor::IronveinAudioProcessor()
    : AudioProcessor (BusesProperties()
          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "IRONVEIN_STATE", createParameterLayout())
{
    rack.addModule (std::make_unique<ironvein::DistortionModule> (parameters));
}

juce::AudioProcessorValueTreeState::ParameterLayout IronveinAudioProcessor::createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<AudioParameterBool> (ParameterID {"dist.enabled", 1}, "Distortion Enabled", true));
    layout.add (std::make_unique<AudioParameterChoice> (ParameterID {"dist.mode", 1}, "Distortion Character",
                                                        StringArray {"Warm", "Hard", "Razor", "Obliterate"}, 1));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.drive", 1}, "Drive",
                                                       NormalisableRange<float> (0.0f, 48.0f, 0.1f), 12.0f, AudioParameterFloatAttributes().withLabel ("dB")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.tight", 1}, "Tight (High Pass)",
                                                       NormalisableRange<float> (35.0f, 350.0f, 1.0f, 0.5f), 100.0f, AudioParameterFloatAttributes().withLabel ("Hz")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.tone", 1}, "Tone (Low Pass)",
                                                       NormalisableRange<float> (1800.0f, 16000.0f, 1.0f, 0.5f), 7500.0f, AudioParameterFloatAttributes().withLabel ("Hz")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.mix", 1}, "Mix",
                                                       NormalisableRange<float> (0.0f, 100.0f, 1.0f), 100.0f, AudioParameterFloatAttributes().withLabel ("%")));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID {"dist.output", 1}, "Output",
                                                       NormalisableRange<float> (-30.0f, 12.0f, 0.1f), -8.0f, AudioParameterFloatAttributes().withLabel ("dB")));
    return layout;
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

void IronveinAudioProcessor::releaseResources()
{
    rack.reset();
}

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

void IronveinAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void IronveinAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType().toString()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IronveinAudioProcessor();
}
