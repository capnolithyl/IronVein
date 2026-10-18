#pragma once
#include <JuceHeader.h>
#include "ForgeLookAndFeel.h"
#include <array>
#include <memory>

namespace ironvein
{
class ParamDial final : public juce::Component
{
public:
    ParamDial (juce::AudioProcessorValueTreeState& parameters,
               const juce::String& parameterId,
               const juce::String& caption,
               const juce::String& explanation);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::String caption;
    juce::Slider knob;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
};

class DistortionPanel final : public juce::Component
{
public:
    explicit DistortionPanel (juce::AudioProcessorValueTreeState&);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateCharacterDescription();
    juce::ComboBox character;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> characterAttachment;
    juce::Label characterDescription;
    ParamDial drive;
    ParamDial tight;
    ParamDial tone;
    ParamDial wetMix;
    ParamDial output;
};
}
