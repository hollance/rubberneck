#pragma once

#include <JuceHeader.h>

class RotaryKnob : public juce::Component
{
public:
    enum Direction : int
    {
        left = 0,
        center = 1,
        right = 2,
    };

    RotaryKnob(Direction direction = Direction::left);

    void setSizes(int knobWidth, int captionHeight = 22, int textBoxHeight = 16);
    void setLabel(const juce::String& label);

    void paint(juce::Graphics&) override;
    void resized() override;

    juce::Slider slider;
    int maxInputLength = 8;

private:
    int knobWidth, knobHeight;
    int captionHeight;
    int textBoxHeight;
    juce::String label;
    Direction direction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnob)
};
