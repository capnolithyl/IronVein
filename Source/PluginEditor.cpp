#include "PluginEditor.h"

IronveinAudioProcessorEditor::IronveinAudioProcessorEditor (IronveinAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      distortionPanel (p.parameters),
      cabinetPanel (p),
      distortionCard (p.parameters, "dist.enabled", "01", "AMP / DISTORTION",
                      "Five original preamp architectures  /  4x oversampling", distortionPanel),
      cabinetCard (p.parameters, "cab.enabled", "02", "CABINET IR",
                   "Speaker simulation  /  bypass for non-guitar tracks", cabinetPanel),
      presets (p)
{
    setLookAndFeel (&appearance);
    addAndMakeVisible (presets);
    advancedBody.addAndMakeVisible (distortionCard);
    advancedBody.addAndMakeVisible (cabinetCard);
    advancedViewport.setScrollBarsShown (true, false);
    advancedViewport.setViewedComponent (&advancedBody, false);
    addChildComponent (advancedViewport);

    advancedButton.setColour (juce::TextButton::buttonColourId, ironvein::Palette::raised());
    advancedButton.setColour (juce::TextButton::textColourOffId, ironvein::Palette::ink());
    advancedButton.onClick = [this] { setAdvancedVisible (! advancedVisible); };
    addAndMakeVisible (advancedButton);

    setResizable (true, true);
    setResizeLimits (900, 740, 1440, 1080);
    setSize (1000, 780);
}

IronveinAudioProcessorEditor::~IronveinAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void IronveinAudioProcessorEditor::setAdvancedVisible (bool show)
{
    advancedVisible = show;
    advancedViewport.setVisible (show);
    advancedButton.setButtonText (show ? "HIDE ADVANCED  ^" : "ADVANCED CONTROLS  v");
    resized();
    repaint();
}

void IronveinAudioProcessorEditor::paint (juce::Graphics& g)
{
    using ironvein::Palette;
    g.fillAll (Palette::background());
    const int w = getWidth(), h = getHeight();

    g.setColour (Palette::accent());
    juce::Path mark;
    mark.addTriangle (27.0f, 29.0f, 48.0f, 29.0f, 37.5f, 54.0f);
    g.fillPath (mark);
    g.setColour (Palette::ink());
    g.setFont (32.0f);
    g.drawText ("IRONVEIN", 63, 18, 290, 45, juce::Justification::centredLeft);
    g.setColour (Palette::accent());
    g.setFont (13.0f);
    g.drawText ("CHOOSE A SOUND. PLAY METAL.", 65, 62, 300, 25,
                juce::Justification::centredLeft);
    g.setColour (Palette::muted());
    g.setFont (13.0f);
    g.drawText ("V0.3  /  PRESET-FIRST GUITAR RIG", w - 360, 31, 336, 35,
                juce::Justification::centredRight);
    g.setColour (Palette::outline());
    g.drawHorizontalLine (101, 23.0f, static_cast<float> (w - 23));

    if (! advancedVisible)
    {
        const auto box = juce::Rectangle<float> (23.0f, 632.0f, static_cast<float> (w - 46), 76.0f);
        g.setColour (Palette::surface());
        g.fillRoundedRectangle (box, 11.0f);
        g.setColour (Palette::outline());
        g.drawRoundedRectangle (box, 11.0f, 1.0f);
        g.setColour (Palette::ink());
        g.setFont (15.0f);
        g.drawText ("READY TO PLAY", 43, 645, 220, 23, juce::Justification::centredLeft);
        g.setColour (Palette::muted());
        g.setFont (13.0f);
        g.drawFittedText ("Select a style and start playing. Advanced gives you individual amp and cabinet controls.",
                          43, 671, w - 95, 22, juce::Justification::centredLeft, 1);
    }
    g.setColour (Palette::muted());
    g.setFont (12.0f);
    g.drawText ("IRONVEIN IS AN EXPERIMENTAL AMP SIM  /  CONTROL YOUR MONITOR VOLUME",
                25, h - 34, w - 50, 22, juce::Justification::centredLeft);
}

void IronveinAudioProcessorEditor::resized()
{
    const int w = getWidth(), h = getHeight();
    presets.setBounds (23, 117, w - 46, 432);
    advancedButton.setBounds (w - 283, 566, 260, 43);
    advancedViewport.setBounds (23, 619, w - 46, juce::jmax (95, h - 660));

    advancedBody.setSize (juce::jmax (820, w - 72), 688);
    distortionCard.setBounds (0, 0, advancedBody.getWidth(), 400);
    cabinetCard.setBounds (0, 411, advancedBody.getWidth(), 270);
}
