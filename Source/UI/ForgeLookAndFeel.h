#pragma once
#include <JuceHeader.h>

namespace ironvein
{
struct Palette
{
    static juce::Colour background() { return juce::Colour::fromRGB (12, 15, 21); }
    static juce::Colour surface()    { return juce::Colour::fromRGB (23, 28, 37); }
    static juce::Colour raised()     { return juce::Colour::fromRGB (31, 38, 49); }
    static juce::Colour outline()    { return juce::Colour::fromRGB (51, 60, 73); }
    static juce::Colour ink()        { return juce::Colour::fromRGB (235, 240, 246); }
    static juce::Colour muted()      { return juce::Colour::fromRGB (140, 151, 166); }
    static juce::Colour accent()     { return juce::Colour::fromRGB (246, 94, 67); }
    static juce::Colour amber()      { return juce::Colour::fromRGB (253, 166, 78); }
};

class ForgeLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    ForgeLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float startAngle, float endAngle,
                           juce::Slider&) override;
    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;
};
}
