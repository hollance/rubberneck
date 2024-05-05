#pragma once

#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour background { 241, 239, 243 };
    const juce::Colour tooltip { 121, 120, 122 };

    namespace Knob
    {
        const juce::Colour trackBackground { 217, 216, 219 };
        const juce::Colour trackActive { 248, 160, 88 };
        const juce::Colour dial { 101, 100, 102 };
        const juce::Colour label { 121, 120, 122 };
        const juce::Colour value { 64, 63, 65 };
        const juce::Colour caret { 0, 0, 0 };
    }

    namespace Button
    {
        const juce::Colour background { 241, 239, 243 };
        const juce::Colour backgroundToggled { 255, 255, 255 };
        const juce::Colour border { 173, 169, 174 };
        const juce::Colour borderToggled { 248, 160, 88 };
        const juce::Colour text { 64, 63, 65 };
        const juce::Colour textToggled { 31, 30, 32 };
    }

    namespace CircleButton
    {
        const juce::Colour background { 121, 120, 122 };
        const juce::Colour backgroundPressed { 64, 63, 65 };
        const juce::Colour text { 241, 239, 243 };
    }

    namespace Analysis
    {
        const juce::Colour background { 255, 255, 255 };
        const juce::Colour border { 217, 216, 219 };
        const juce::Colour text { 121, 120, 122 };
    }
}

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
    RotaryKnobLookAndFeel();

    static RotaryKnobLookAndFeel* get()
    {
        static RotaryKnobLookAndFeel instance;
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

class PickerButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PickerButtonLookAndFeel();

    static PickerButtonLookAndFeel* get()
    {
        static PickerButtonLookAndFeel instance;
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PickerButtonLookAndFeel)
};

class CircleButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CircleButtonLookAndFeel();

    static CircleButtonLookAndFeel* get()
    {
        static CircleButtonLookAndFeel instance;
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CircleButtonLookAndFeel)
};
