#include "CabinetPanel.h"
#include "../PluginProcessor.h"

namespace ironvein
{
CabinetPanel::CabinetPanel (IronveinAudioProcessor& p)
    : processor (p),
      output (p.parameters, "cab.level", "Level", "Trim the loaded cab response to match volume. Start at -6 dB.")
{
    selection.addItemList ({"1x12 Open", "2x12 Closed", "4x12 Straight", "4x12 Slant", "Custom WAV"}, 1);
    selection.setColour (juce::ComboBox::textColourId, Palette::ink());
    selection.setColour (juce::ComboBox::backgroundColourId, Palette::raised());
    selection.setColour (juce::ComboBox::outlineColourId, Palette::outline());
    selectionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        p.parameters, "cab.ir", selection);
    selection.onChange = [this] { updateFileStatus(); };
    addAndMakeVisible (selection);

    loadButton.setColour (juce::TextButton::buttonColourId, Palette::raised());
    loadButton.setColour (juce::TextButton::textColourOffId, Palette::ink());
    loadButton.onClick = [this] { chooseWav(); };
    addAndMakeVisible (loadButton);

    fileStatus.setColour (juce::Label::textColourId, Palette::muted());
    fileStatus.setFont (juce::Font (juce::FontOptions (12.5f)));
    addAndMakeVisible (fileStatus);
    addAndMakeVisible (output);
    updateFileStatus();
}

void CabinetPanel::chooseWav()
{
    chooser = std::make_unique<juce::FileChooser> ("Select cabinet impulse response (WAV)",
                                                   juce::File{}, "*.wav");
    juce::Component::SafePointer<CabinetPanel> safe (this);
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                          [safe] (const juce::FileChooser& picker)
                          {
                              if (safe == nullptr)
                                  return;
                              const auto file = picker.getResult();
                              if (safe->processor.loadCustomCabinetIR (file))
                              {
                                  safe->selection.setSelectedId (5, juce::sendNotificationSync);
                                  safe->updateFileStatus();
                              }
                              else
                              {
                                  safe->fileStatus.setText ("Invalid WAV or file exceeds 32 MB",
                                                            juce::dontSendNotification);
                              }
                          });
}

void CabinetPanel::updateFileStatus()
{
    const auto custom = selection.getSelectedId() == 5;
    fileStatus.setText (custom ? processor.getCustomCabinetName()
                               : "Bundled CC0 cabinet response  /  load your own WAV at any time",
                        juce::dontSendNotification);
}

void CabinetPanel::paint (juce::Graphics& g)
{
    g.setColour (Palette::muted());
    g.setFont (12.5f);
    g.drawText ("CABINET", 1, 2, 100, 28, juce::Justification::centredLeft);
    g.setColour (Palette::outline());
    g.drawHorizontalLine (48, 0.0f, static_cast<float> (getWidth()));
    g.setColour (Palette::muted());
    g.setFont (12.0f);
    g.drawFittedText ("CONVOLUTION  /  4 BUILT-IN CAB IRs  /  YOUR OWN WAV",
                      2, getHeight() - 24, getWidth() - 4, 19,
                      juce::Justification::centredLeft, 1);
}

void CabinetPanel::resized()
{
    selection.setBounds (110, 0, juce::jmin (270, getWidth() / 3), 34);
    loadButton.setBounds (selection.getRight() + 14, 0, 126, 34);
    fileStatus.setBounds (2, 52, juce::jmax (240, getWidth() - 200), 36);
    output.setBounds (getWidth() - 142, 41, 132, juce::jmax (112, getHeight() - 65));
}
}
