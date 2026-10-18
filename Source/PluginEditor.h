#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/ForgeLookAndFeel.h"
#include "UI/ModuleCard.h"
#include "UI/DistortionPanel.h"
#include "UI/CabinetPanel.h"
#include "UI/PresetPanel.h"

class IronveinAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit IronveinAudioProcessorEditor (IronveinAudioProcessor&);
    ~IronveinAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setAdvancedVisible (bool show);
    IronveinAudioProcessor& processor;
    ironvein::ForgeLookAndFeel appearance;
    ironvein::DistortionPanel distortionPanel;
    ironvein::CabinetPanel cabinetPanel;
    ironvein::ModuleCard distortionCard;
    ironvein::ModuleCard cabinetCard;
    ironvein::PresetPanel presets;
    juce::Component advancedBody;
    juce::Viewport advancedViewport;
    juce::TextButton advancedButton {"ADVANCED CONTROLS  v"};
    juce::TooltipWindow tooltip {this, 650};
    bool advancedVisible = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IronveinAudioProcessorEditor)
};
