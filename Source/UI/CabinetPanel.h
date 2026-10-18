#pragma once
#include <JuceHeader.h>
#include "ForgeLookAndFeel.h"
#include "DistortionPanel.h"
#include <memory>

class IronveinAudioProcessor;

namespace ironvein
{
class CabinetPanel final : public juce::Component
{
public:
    explicit CabinetPanel (IronveinAudioProcessor&);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateFileStatus();
    void chooseWav();

    IronveinAudioProcessor& processor;
    juce::ComboBox selection;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> selectionAttachment;
    juce::TextButton loadButton {"LOAD WAV"};
    juce::Label fileStatus;
    ParamDial output;
    std::unique_ptr<juce::FileChooser> chooser;
};
}
