#include "PluginEditor.h"

IronveinAudioProcessorEditor::IronveinAudioProcessorEditor (IronveinAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      distortionPanel (p.parameters),
      cabinetPanel (p),
      distortionCard (p.parameters, "dist.enabled", "01", "DISTORTION",
                      "Five metal voicings  /  4x oversampling", distortionPanel),
      cabinetCard (p.parameters, "cab.enabled", "02", "CABINET IR",
                   "Guitar speaker simulation  /  optional for buses", cabinetPanel)
{
    setLookAndFeel (&appearance);
    addAndMakeVisible (distortionCard);
    addAndMakeVisible (cabinetCard);
    setSize (1030, 820);
    setResizable (true, true);
    setResizeLimits (900, 780, 1400, 1050);
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
    g.drawText ("V0.2  /  FIVE VOICES  /  FOUR CAB IRs", w - 364, 55, 335, 22,
                juce::Justification::centredRight);
    g.setColour (Palette::outline());
    g.drawHorizontalLine (98, 24.0f, static_cast<float> (w - 24));

    g.setColour (Palette::muted());
    g.setFont (13.0f);
    g.drawText ("+  NEXT UP   EQ  /  COMPRESSION  /  REVERB  /  DELAY",
                28, h - 39, w - 56, 26, juce::Justification::centredLeft);
}

void IronveinAudioProcessorEditor::resized()
{
    const auto contentWidth = getWidth() - 46;
    distortionCard.setBounds (23, 114, contentWidth, 400);
    cabinetCard.setBounds (23, 526, contentWidth, getHeight() - 595);
}
