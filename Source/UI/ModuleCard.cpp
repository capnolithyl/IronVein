#include "ModuleCard.h"
#include <utility>

namespace ironvein
{
void PowerToggle::paintButton (juce::Graphics& g, bool, bool)
{
    const auto active = getToggleState();
    auto rect = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (active ? Palette::accent().withAlpha (0.13f) : Palette::raised());
    g.fillRoundedRectangle (rect, 9.0f);
    g.setColour (active ? Palette::accent() : Palette::outline());
    g.drawRoundedRectangle (rect, 9.0f, 1.1f);

    g.setColour (active ? Palette::accent() : Palette::muted());
    g.fillEllipse (14.0f, rect.getCentreY() - 4.0f, 8.0f, 8.0f);
    g.setFont (13.0f);
    g.drawText (active ? "ENABLED" : "BYPASSED", 32, 0, getWidth() - 38,
                getHeight(), juce::Justification::centredLeft);
}

ModuleCard::ModuleCard (juce::AudioProcessorValueTreeState& parameters,
                        const juce::String& enabledParameterId,
                        juce::String slotNumber,
                        juce::String moduleTitle,
                        juce::String moduleSubtitle,
                        juce::Component& moduleContent)
    : number (std::move (slotNumber)),
      title (std::move (moduleTitle)),
      subtitle (std::move (moduleSubtitle)),
      content (moduleContent),
      powerAttachment (parameters, enabledParameterId, power)
{
    addAndMakeVisible (power);
    addAndMakeVisible (content);
    power.setTooltip ("Enable or bypass this effect. A short crossfade reduces switching clicks.");
}

void ModuleCard::paint (juce::Graphics& g)
{
    const auto area = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (Palette::surface());
    g.fillRoundedRectangle (area, 14.0f);
    g.setColour (Palette::outline());
    g.drawRoundedRectangle (area, 14.0f, 1.0f);

    g.setColour (Palette::accent());
    g.fillRoundedRectangle (24.0f, 23.0f, 5.0f, 48.0f, 2.5f);
    g.setColour (Palette::muted());
    g.setFont (13.0f);
    g.drawText ("MODULE " + number, 44, 18, 125, 23, juce::Justification::centredLeft);
    g.setColour (Palette::ink());
    g.setFont (25.0f);
    g.drawText (title, 44, 42, getWidth() - 290, 33, juce::Justification::centredLeft);
    g.setColour (Palette::muted());
    g.setFont (13.0f);
    g.drawText (subtitle, 45, 79, getWidth() - 90, 20, juce::Justification::centredLeft);

    g.setColour (Palette::outline());
    g.drawHorizontalLine (110, 24.0f, static_cast<float> (getWidth() - 24));
}

void ModuleCard::resized()
{
    power.setBounds (getWidth() - 167, 32, 140, 38);
    content.setBounds (26, 121, getWidth() - 52, getHeight() - 140);
}
}
