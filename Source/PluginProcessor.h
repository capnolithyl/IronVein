#pragma once
#include <JuceHeader.h>
#include "Presets/FactoryPresets.h"
#include "DSP/EffectRack.h"
#include "DSP/DistortionModule.h"
#include "DSP/CabinetModule.h"

class IronveinAudioProcessor final : public juce::AudioProcessor
{
public:
    IronveinAudioProcessor();
    ~IronveinAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool loadCustomCabinetIR (const juce::File& file);
    juce::String getCustomCabinetName() const;
    void applyFactoryPreset (int index);
    int getSelectedFactoryPreset() const noexcept;

    juce::AudioProcessorValueTreeState parameters;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void setRealParameter (const juce::String& id, float value);

    ironvein::EffectRack rack;
    ironvein::CabinetModule* cabinet = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IronveinAudioProcessor)
};
