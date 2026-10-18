#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/ForgeLookAndFeel.h"
#include "UI/ModuleCard.h"
#include "UI/DistortionPanel.h"
#include "UI/CabinetPanel.h"

class IronveinAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit IronveinAudioProcessorEditor (IronveinAudioProcessor&);
    ~IronveinAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    IronveinAudioProcessor& processor;
    ironvein::ForgeLookAndFeel appearance;
    ironvein::DistortionPanel distortionPanel;
    ironvein::CabinetPanel cabinetPanel;
    ironvein::ModuleCard distortionCard;
    ironvein::ModuleCard cabinetCard;
    juce::TooltipWindow tooltip {this, 650};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IronveinAudioProcessorEditor)
};
