#include "ForgeLookAndFeel.h"
#include <cmath>

namespace ironvein
{
ForgeLookAndFeel::ForgeLookAndFeel()
{
    setColour (juce::PopupMenu::backgroundColourId, Palette::raised());
    setColour (juce::PopupMenu::textColourId, Palette::ink());
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Palette::accent());
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

void ForgeLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float startAngle, float endAngle, juce::Slider&)
{
    const auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y),
                                                static_cast<float> (width), static_cast<float> (height));
    const auto centre = bounds.getCentre();
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.35f;
    const auto angle = startAngle + sliderPos * (endAngle - startAngle);

    juce::Path track, active;
    track.addCentredArc (centre.x, centre.y, radius, radius, 0.0f,
                         startAngle, endAngle, true);
    active.addCentredArc (centre.x, centre.y, radius, radius, 0.0f,
                          startAngle, angle, true);
    g.setColour (Palette::outline());
    g.strokePath (track, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));
    g.setColour (Palette::accent());
    g.strokePath (active, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    g.setColour (Palette::raised());
    g.fillEllipse (centre.x - radius * 0.73f, centre.y - radius * 0.73f,
                   radius * 1.46f, radius * 1.46f);
    g.setColour (Palette::outline().brighter (0.25f));
    g.drawEllipse (centre.x - radius * 0.73f, centre.y - radius * 0.73f,
                   radius * 1.46f, radius * 1.46f, 1.0f);

    juce::Path indicator;
    const auto length = radius * 0.58f;
    indicator.addLineSegment (juce::Line<float> (centre.x + std::sin (angle) * radius * 0.17f,
                                                 centre.y - std::cos (angle) * radius * 0.17f,
                                                 centre.x + std::sin (angle) * length,
                                                 centre.y - std::cos (angle) * length), 3.0f);
    g.setColour (Palette::ink());
    g.fillPath (indicator);
}

void ForgeLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                    int, int, int, int, juce::ComboBox&)
{
    auto rect = juce::Rectangle<float> (0.5f, 0.5f,
                                        static_cast<float> (width) - 1.0f,
                                        static_cast<float> (height) - 1.0f);
    g.setColour (Palette::raised());
    g.fillRoundedRectangle (rect, 7.0f);
    g.setColour (Palette::outline());
    g.drawRoundedRectangle (rect, 7.0f, 1.0f);
    const auto xx = static_cast<float> (width - 21);
    const auto yy = static_cast<float> (height / 2);
    juce::Path arrow;
    arrow.startNewSubPath (xx - 5.0f, yy - 2.0f);
    arrow.lineTo (xx, yy + 3.0f);
    arrow.lineTo (xx + 5.0f, yy - 2.0f);
    g.setColour (Palette::accent());
    g.strokePath (arrow, juce::PathStrokeType (2.0f));
}
}
