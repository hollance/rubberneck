#pragma once

#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour background { 245, 240, 235 };
    const juce::Colour tooltip { 120, 120, 120 };

    namespace Knob
    {
        const juce::Colour trackBackground { 205, 200, 195 };
        const juce::Colour trackActive { 50, 200, 220 };
        const juce::Colour trackFromCenter { 213, 116, 212 };
        const juce::Colour outline { 255, 250, 245 };
        const juce::Colour gradientTop { 250, 245, 240 };
        const juce::Colour gradientBottom { 240, 235, 230 };
        const juce::Colour dial { 100, 100, 100 };
        const juce::Colour dropShadow { 195, 190, 185 };
        const juce::Colour label { 80, 80, 80 };
        const juce::Colour textBoxBackground { 80, 80, 80 };
        const juce::Colour value { 240, 240, 240 };
        const juce::Colour caret { 255, 255, 255 };
    };

    namespace Button
    {
        const juce::Colour background { 245, 240, 235 };
        const juce::Colour backgroundToggled { 40, 120, 140 };
        const juce::Colour border { 40, 120, 140 };
        const juce::Colour borderPressed { 25, 80, 95 };
        const juce::Colour text { 25, 80, 95 };
        const juce::Colour textToggled { 255, 255, 255 };
        const juce::Colour textPressed { 10, 35, 40 };
    };
};

class Fonts
{
public:
    static juce::Font getFont(float height = 15.0f);

private:
    static const juce::Typeface::Ptr typeface;
};

class RotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RotaryKnobLookAndFeel(bool fromCenter = false);

    static RotaryKnobLookAndFeel* get()
    {
        static RotaryKnobLookAndFeel instance;
        return &instance;
    }

    static RotaryKnobLookAndFeel* getCentered()
    {
        static RotaryKnobLookAndFeel instance { true };
        return &instance;
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    juce::Label* createSliderTextBox(juce::Slider&) override;
    juce::Font getLabelFont(juce::Label&) override;
    void drawLabel(juce::Graphics&, juce::Label&) override;
    void fillTextEditorBackground(juce::Graphics&, int width, int height, juce::TextEditor&) override;
    void drawTextEditorOutline(juce::Graphics&, int width, int height, juce::TextEditor&) override;

protected:
    bool fromCenter;
    juce::DropShadow dropShadow;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};

class ButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ButtonLookAndFeel();

    static ButtonLookAndFeel* get()
    {
        static ButtonLookAndFeel instance;
        return &instance;
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonLookAndFeel)
};
