#include "PresetPanel.h"
#include "../PluginProcessor.h"

namespace ironvein
{
PresetPanel::PresetPanel (IronveinAudioProcessor& p)
    : processor (p),
      gain (p.parameters, "macro.gain", "MORE / LESS GAIN", "Add or subtract up to 12 dB of drive without losing the preset."),
      tightness (p.parameters, "macro.tight", "LOOSE / TIGHT", "More cuts bass before distortion; less keeps extra low end."),
      brightness (p.parameters, "macro.bright", "DARK / BRIGHT", "Adjust the treble rolloff while keeping the voicing's character.")
{
    for (int i = 0; i < static_cast<int> (factoryPresets.size()); ++i)
        presets.addItem (factoryPresets[static_cast<size_t> (i)].name, i + 1);
    presets.setJustificationType (juce::Justification::centredLeft);
    presets.setColour (juce::ComboBox::textColourId, Palette::ink());
    presets.setColour (juce::ComboBox::backgroundColourId, Palette::raised());
    presets.setColour (juce::ComboBox::outlineColourId, Palette::outline());
    presets.onChange = [this]
    {
        const int i = presets.getSelectedId() - 1;
        if (i >= 0)
        {
            processor.applyFactoryPreset (i);
            refresh();
        }
    };
    presets.setTooltip ("Choose a complete guitar tone: amp, clipping, filters, levels and cabinet.");
    addAndMakeVisible (presets);

    reset.setColour (juce::TextButton::buttonColourId, Palette::raised());
    reset.setColour (juce::TextButton::textColourOffId, Palette::ink());
    reset.setTooltip ("Reapply this factory rig, resetting all detailed controls and quick tweaks.");
    reset.onClick = [this]
    {
        const int i = processor.getSelectedFactoryPreset();
        if (i >= 0)
            processor.applyFactoryPreset (i);
        refresh();
    };
    addAndMakeVisible (reset);

    for (auto* label : {&description, &usage, &route})
    {
        label->setColour (juce::Label::textColourId, Palette::muted());
        label->setFont (juce::Font (juce::FontOptions (14.0f)));
        label->setJustificationType (juce::Justification::centredLeft);
        addAndMakeVisible (*label);
    }
    for (auto* macro : {&gain, &tightness, &brightness})
        addAndMakeVisible (*macro);
    refresh();
    startTimerHz (4); // host session restore may happen while editor stays open
}

void PresetPanel::refresh()
{
    const int i = processor.getSelectedFactoryPreset();
    if (i == displayedPreset)
        return;
    displayedPreset = i;
    if (i >= 0 && i < static_cast<int> (factoryPresets.size()))
    {
        const auto& p = factoryPresets[static_cast<size_t> (i)];
        presets.setSelectedId (i + 1, juce::dontSendNotification);
        description.setText (p.description, juce::dontSendNotification);
        usage.setText (juce::String ("IDEAL FOR    ") + p.recommendedUse, juce::dontSendNotification);
        const juce::String engines[] = {"Transistor fuzz", "Chainsaw pedal", "Vintage preamp",
                                         "High-gain preamp", "Boosted modern preamp"};
        const juce::String cabinets[] = {"1x12 Open", "2x12 Closed", "4x12 Straight", "4x12 Slant"};
        route.setText (engines[p.engine] + "    >    " + cabinets[p.cabinet],
                       juce::dontSendNotification);
        reset.setEnabled (true);
    }
    else
    {
        presets.setSelectedId (0, juce::dontSendNotification);
        presets.setText ("Custom / imported session", juce::dontSendNotification);
        description.setText ("Your saved sound is preserved. Choose a preset to replace it.",
                             juce::dontSendNotification);
        usage.setText ("Existing v0.2 sessions retain their saved module values.",
                       juce::dontSendNotification);
        route.setText ("CUSTOM SIGNAL CHAIN", juce::dontSendNotification);
        reset.setEnabled (false);
    }
    repaint();
}

void PresetPanel::timerCallback() { refresh(); }

void PresetPanel::paint (juce::Graphics& g)
{
    const auto area = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (Palette::surface());
    g.fillRoundedRectangle (area, 15.0f);
    g.setColour (Palette::outline());
    g.drawRoundedRectangle (area, 15.0f, 1.0f);

    g.setColour (Palette::accent());
    g.fillRoundedRectangle (25.0f, 22.0f, 4.0f, 30.0f, 2.0f);
    g.setColour (Palette::ink());
    g.setFont (20.0f);
    g.drawText ("CHOOSE YOUR SOUND", 40, 16, 420, 34,
                juce::Justification::centredLeft);
    g.setColour (Palette::outline());
    g.drawHorizontalLine (185, 22.0f, static_cast<float> (getWidth() - 22));
    g.setColour (Palette::muted());
    g.setFont (12.0f);
    g.drawText ("THREE TWEAKS, NO ENGINEERING DEGREE REQUIRED",
                30, 195, getWidth() - 60, 25, juce::Justification::centredLeft);
    g.setColour (Palette::outline());
    g.drawHorizontalLine (getHeight() - 62, 22.0f, static_cast<float> (getWidth() - 22));
    g.setColour (Palette::muted());
    g.setFont (12.0f);
    g.drawText ("SIGNAL PATH", 29, getHeight() - 55, 125, 26,
                juce::Justification::centredLeft);
}

void PresetPanel::resized()
{
    const int w = getWidth(), h = getHeight();
    presets.setBounds (29, 60, juce::jmax (300, w - 291), 38);
    reset.setBounds (w - 246, 60, 217, 38);
    description.setBounds (29, 103, w - 58, 30);
    usage.setBounds (29, 136, w - 58, 36);
    const int knobTop = 227;
    const int knobH = juce::jmax (110, h - knobTop - 74);
    const int knobW = (w - 58) / 3;
    gain.setBounds (29, knobTop, knobW, knobH);
    tightness.setBounds (29 + knobW, knobTop, knobW, knobH);
    brightness.setBounds (29 + 2 * knobW, knobTop, knobW, knobH);
    route.setBounds (154, h - 55, w - 180, 26);
}
}
