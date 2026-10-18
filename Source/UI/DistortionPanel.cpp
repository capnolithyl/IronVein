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
    : drive (parameters, "dist.drive", "Drive", "Input boost before clipping: 0 to +48 dB."),
      tight (parameters, "dist.tight", "Tight", "Pre-distortion high-pass: cut sub-bass before saturation (35 to 350 Hz)."),
      tone (parameters, "dist.tone", "Tone", "Post-distortion low-pass: reduce fizz above this frequency."),
      wetMix (parameters, "dist.mix", "Mix", "Blend processed and latency-aligned clean audio (0 to 100%)."),
      output (parameters, "dist.output", "Output", "Post-effect trim: match loudness, protect your next processor.")
{
    character.addItemList ({"Warm", "Hard", "Razor", "Obliterate"}, 1);
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
        case 1: characterDescription.setText ("Rounded saturation  /  lighter texture", juce::dontSendNotification); break;
        case 2: characterDescription.setText ("Hard clipping  /  raw crunch", juce::dontSendNotification); break;
        case 3: characterDescription.setText ("Uneven clipping  /  sharp harmonics", juce::dontSendNotification); break;
        case 4: characterDescription.setText ("High-density saturation  /  maximum filth", juce::dontSendNotification); break;
        default: characterDescription.setText ("Pick your flavor of damage", juce::dontSendNotification); break;
    }
}

void DistortionPanel::paint (juce::Graphics& g)
{
    g.setColour (Palette::muted());
    g.setFont (12.5f);
    g.drawText ("CHARACTER", 2, 9, 95, 32, juce::Justification::centredLeft);
    g.setColour (Palette::outline());
    g.drawHorizontalLine (58, 0.0f, static_cast<float> (getWidth()));

    const auto y = getHeight() - 25;
    g.setColour (Palette::muted());
    g.setFont (12.0f);
    g.drawFittedText ("SIGNAL  >  PRE-HIGH-PASS  >  CLIPPER (4x)  >  DC FILTER  >  LOW-PASS  >  MIX / OUTPUT",
                      2, y, getWidth() - 4, 20, juce::Justification::centred, 1);
}

void DistortionPanel::resized()
{
    character.setBounds (106, 7, juce::jmin (240, getWidth() / 3), 36);
    characterDescription.setBounds (character.getRight() + 17, 7,
                                    juce::jmax (50, getWidth() - character.getRight() - 20), 36);
    const auto top = 65;
    const auto dialHeight = juce::jmax (148, getHeight() - top - 30);
    const auto dialWidth = getWidth() / 5;
    std::array<ParamDial*, 5> dials {&drive, &tight, &tone, &wetMix, &output};
    for (int i = 0; i < 5; ++i)
        dials[static_cast<size_t> (i)]->setBounds (i * dialWidth, top,
                     i == 4 ? getWidth() - (i * dialWidth) : dialWidth, dialHeight);
}
}
