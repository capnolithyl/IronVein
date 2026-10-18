#pragma once
#include <JuceHeader.h>
#include "DistortionPanel.h"
#include "ForgeLookAndFeel.h"

class IronveinAudioProcessor;

namespace ironvein
{
class PresetPanel final : public juce::Component, private juce::Timer
{
public:
    explicit PresetPanel (IronveinAudioProcessor&);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void refresh();
    void timerCallback() override;
    IronveinAudioProcessor& processor;
    juce::ComboBox presets;
    juce::Label description, usage, route;
    juce::TextButton reset {"RESTORE PRESET"};
    ParamDial gain, tightness, brightness;
    int displayedPreset = -99;
};
}
