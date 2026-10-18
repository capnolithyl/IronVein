#include "PluginEditor.h"

IronveinAudioProcessorEditor::IronveinAudioProcessorEditor (IronveinAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      distortionPanel (p.parameters),
      distortionCard (p.parameters, "dist.enabled", "01", "DISTORTION",
                      "Gain  /  Harmonics  /  Control", distortionPanel)
{
    setLookAndFeel (&appearance);
    addAndMakeVisible (distortionCard);
    setSize (1020, 650);
    setResizable (true, true);
    setResizeLimits (850, 600, 1340, 830);
}

IronveinAudioProcessorEditor::~IronveinAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void IronveinAudioProcessorEditor::paint (juce::Graphics& g)
{
    using ironvein::Palette;
    g.fillAll (Palette::background());
    const auto w = getWidth();
    const auto h = getHeight();

    // Vector artwork is DPI-independent and needs no external images.
    g.setColour (Palette::accent());
    juce::Path mark;
    mark.addTriangle (27.0f, 29.0f, 48.0f, 29.0f, 37.5f, 54.0f);
    g.fillPath (mark);
    g.setColour (Palette::ink());
    g.setFont (32.0f);
    g.drawText ("IRONVEIN", 63, 18, 290, 45, juce::Justification::centredLeft);
    g.setColour (Palette::accent());
    g.setFont (13.0f);
    g.drawText ("MODULAR FX RACK", 65, 62, 260, 25, juce::Justification::centredLeft);

    g.setColour (Palette::muted());
    g.setFont (13.0f);
    g.drawText ("INSERT EFFECTS    /    MONO + STEREO", w - 364, 28, 335, 24,
                juce::Justification::centredRight);
    g.drawText ("V0.1   •   DSP BUILT FROM SCRATCH", w - 364, 55, 335, 22,
                juce::Justification::centredRight);
    g.setColour (Palette::outline());
    g.drawHorizontalLine (98, 24.0f, static_cast<float> (w - 24));

    const auto foot = juce::Rectangle<float> (23.0f, static_cast<float> (h - 111),
                                              static_cast<float> (w - 46), 87.0f);
    g.setColour (Palette::surface());
    g.fillRoundedRectangle (foot, 12.0f);
    g.setColour (Palette::outline());
    g.drawRoundedRectangle (foot, 12.0f, 1.0f);
    g.setColour (Palette::muted());
    g.setFont (13.0f);
    g.drawText ("+  SLOT 02", 46, h - 95, 140, 27, juce::Justification::centredLeft);
    g.setColour (Palette::ink());
    g.setFont (17.0f);
    g.drawText ("A bigger chain is coming.", 46, h - 71, 390, 27,
                juce::Justification::centredLeft);
    g.setColour (Palette::muted());
    g.setFont (13.0f);
    g.drawText ("NEXT UP     EQ    /    COMPRESSOR    /    DELAY    /    REVERB",
                w - 495, h - 82, 465, 30, juce::Justification::centredRight);
}

void IronveinAudioProcessorEditor::resized()
{
    distortionCard.setBounds (23, 114, getWidth() - 46, getHeight() - 241);
}
