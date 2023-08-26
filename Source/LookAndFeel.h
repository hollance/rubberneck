#pragma once

#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour background { 245, 240, 235 };
    const juce::Colour tooltip { 120, 120, 120 };

    namespace Knob
    {
        const juce::Colour trackBackground { 205, 200, 195 };
        const juce::Colour trackActive { 248, 160, 88 };
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

    namespace BarButton
    {
        const juce::Colour background { 248, 160, 88 };
        const juce::Colour backgroundToggled { 255, 255, 255 };
        const juce::Colour borderToggled { 255, 255, 255 };
        const juce::Colour border { 255, 255, 255 };
        const juce::Colour text { 255, 255, 255 };
        const juce::Colour textToggled { 248, 160, 88 };
    };

    namespace Button
    {
        const juce::Colour text { 80, 80, 80 };
        const juce::Colour textToggled { 255, 255, 255 };
        const juce::Colour gradientTopToggled { 248, 160, 88 };
        const juce::Colour gradientBottomToggled  { 255, 195, 140 };
    };

    namespace RoundButton
    {
        const juce::Colour background { 100, 100, 100 };
        const juce::Colour backgroundPressed { 80, 80, 80 };
        const juce::Colour text { 240, 240, 240 };
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

class BarButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BarButtonLookAndFeel();

    static BarButtonLookAndFeel* get()
    {
        static BarButtonLookAndFeel instance;
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BarButtonLookAndFeel)
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
    juce::DropShadow dropShadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonLookAndFeel)
};

class RoundButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RoundButtonLookAndFeel();

    static RoundButtonLookAndFeel* get()
    {
        static RoundButtonLookAndFeel instance;
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoundButtonLookAndFeel)
};
