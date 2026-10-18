#pragma once
#include <JuceHeader.h>
#include "ForgeLookAndFeel.h"
#include <memory>

namespace ironvein
{
class PowerToggle final : public juce::ToggleButton
{
public:
    void paintButton (juce::Graphics&, bool hovered, bool down) override;
};

// Reusable visual container for future EQ, dynamics, delay, reverb etc.
class ModuleCard final : public juce::Component
{
public:
    ModuleCard (juce::AudioProcessorValueTreeState& parameters,
                const juce::String& enabledParameterId,
                juce::String number,
                juce::String title,
                juce::String subtitle,
                juce::Component& moduleContent);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::String number, title, subtitle;
    juce::Component& content;
    PowerToggle power;
    juce::AudioProcessorValueTreeState::ButtonAttachment powerAttachment;
};
}
