#include "DistortionPanel.h"

namespace ironvein
{
ParamDial::ParamDial (juce::AudioProcessorValueTreeState& parameters,
                      const juce::String& parameterId,
                      const juce::String& title,
                      const juce::String& explanation)
    : caption (title), attachment (parameters, parameterId, knob)
{
    addAndMakeVisible (knob);
    knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 112, 24);
    knob.setColour (juce::Slider::textBoxTextColourId, Palette::ink());
    knob.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    knob.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    knob.setColour (juce::Slider::textBoxHighlightColourId, Palette::accent());
    knob.setTooltip (explanation);
}

void ParamDial::paint (juce::Graphics& g)
{
    g.setColour (Palette::muted());
    g.setFont (13.0f);
    g.drawFittedText (caption.toUpperCase(), getLocalBounds().removeFromTop (22),
                      juce::Justification::centred, 1);
}

void ParamDial::resized()
{
    knob.setBounds (0, 22, getWidth(), getHeight() - 22);
}

DistortionPanel::DistortionPanel (juce::AudioProcessorValueTreeState& parameters)
    : drive (parameters, "dist.drive", "Drive", "Boost before clipping (0 to 48 dB). Start around 8-18 dB: 48 dB slams every voicing."),
      tight (parameters, "dist.tight", "Tight", "Additional low-end cut before gain. Each voicing also has a built-in high pass."),
      tone (parameters, "dist.tone", "Tone", "Additional treble roll-off after distortion."),
      wetMix (parameters, "dist.mix", "Mix", "Blend processed and aligned unprocessed signal."),
      output (parameters, "dist.output", "Output", "Post-distortion output level. Lower it to avoid clipping the next module.")
{
    character.addItemList ({"Raw Black Metal", "Swedish Chainsaw", "Old-School Heavy",
                            "Modern Death Metal", "Modern Metalcore"}, 1);
    characterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        parameters, "dist.mode", character);
    character.setColour (juce::ComboBox::textColourId, Palette::ink());
    character.setColour (juce::ComboBox::backgroundColourId, Palette::raised());
    character.setColour (juce::ComboBox::outlineColourId, Palette::outline());
    character.setJustificationType (juce::Justification::centredLeft);
    character.onChange = [this] { updateCharacterDescription(); };
    addAndMakeVisible (character);

    characterDescription.setColour (juce::Label::textColourId, Palette::muted());
    characterDescription.setJustificationType (juce::Justification::centredLeft);
    characterDescription.setFont (juce::Font (juce::FontOptions (13.0f)));
    addAndMakeVisible (characterDescription);

    for (auto* dial : {&drive, &tight, &tone, &wetMix, &output})
        addAndMakeVisible (*dial);
    updateCharacterDescription();
}

void DistortionPanel::updateCharacterDescription()
{
    switch (character.getSelectedId())
    {
        case 1: characterDescription.setText ("Abrasive upper mids  /  raw and cold", juce::dontSendNotification); break;
        case 2: characterDescription.setText ("HM-2-inspired midrange grind  /  chainsaw", juce::dontSendNotification); break;
        case 3: characterDescription.setText ("Open crunch  /  pick-sensitive grit", juce::dontSendNotification); break;
        case 4: characterDescription.setText ("Dense attack  /  classic death-metal weight", juce::dontSendNotification); break;
        case 5: characterDescription.setText ("Precise, tightly filtered modern gain", juce::dontSendNotification); break;
        default: characterDescription.setText ("Select a metal voicing", juce::dontSendNotification); break;
    }
}

void DistortionPanel::paint (juce::Graphics& g)
{
    g.setColour (Palette::muted());
    g.setFont (12.5f);
    g.drawText ("VOICING", 2, 9, 95, 32, juce::Justification::centredLeft);
    g.setColour (Palette::outline());
    g.drawHorizontalLine (58, 0.0f, static_cast<float> (getWidth()));
    g.setColour (Palette::muted());
    g.setFont (12.0f);
    g.drawFittedText ("SIGNAL  >  PRE-EQ  >  MULTISTAGE CLIPPER (4x)  >  POST-EQ  >  MIX / OUTPUT",
                      2, getHeight() - 25, getWidth() - 4, 20,
                      juce::Justification::centred, 1);
}

void DistortionPanel::resized()
{
    character.setBounds (106, 7, juce::jmin (260, getWidth() / 3), 36);
    characterDescription.setBounds (character.getRight() + 17, 7,
                                    juce::jmax (50, getWidth() - character.getRight() - 20), 36);
    const auto top = 65;
    const auto dialHeight = juce::jmax (140, getHeight() - top - 30);
    const auto dialWidth = getWidth() / 5;
    std::array<ParamDial*, 5> dials {&drive, &tight, &tone, &wetMix, &output};
    for (int i = 0; i < 5; ++i)
        dials[static_cast<size_t> (i)]->setBounds (i * dialWidth, top,
                    i == 4 ? getWidth() - (i * dialWidth) : dialWidth, dialHeight);
}
}
