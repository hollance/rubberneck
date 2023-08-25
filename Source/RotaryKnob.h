#pragma once

#include <JuceHeader.h>

class RotaryKnob : public juce::Component
{
public:
    RotaryKnob();

    void setLabel(const juce::String& label);
    void setColor(const juce::Colour& color);
    void setFont(const juce::Font& font);

    void paint(juce::Graphics&) override;
    void resized() override;

    juce::Slider slider;

private:
    juce::String label;
    juce::Colour color;
    juce::Font font;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnob)
};
