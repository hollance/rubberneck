#include "RotaryKnob.h"

static constexpr float pi = juce::MathConstants<float>::pi;

RotaryKnob::RotaryKnob()
{
    setSizes(80, 22, 16);
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setRotaryParameters(1.25f * pi, 2.75f * pi, true);
    addAndMakeVisible(slider);
}

void RotaryKnob::setSizes(int newKnobWidth, int newCaptionHeight, int newTextBoxHeight)
{
    knobWidth = newKnobWidth;
    knobHeight = newKnobWidth;
    captionHeight = newCaptionHeight;
    textBoxHeight = newTextBoxHeight;

    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, knobWidth, textBoxHeight);
    setBounds(0, 0, knobWidth, captionHeight + knobHeight + textBoxHeight);
}

void RotaryKnob::setLabel(const juce::String& newLabel)
{
    label = newLabel;
    slider.setTitle(label);
}

void RotaryKnob::setColor(const juce::Colour& newColor)
{
    color = newColor;
}

void RotaryKnob::setFont(const juce::Font& newFont)
{
    font = newFont;
}

void RotaryKnob::paint(juce::Graphics& g)
{
    g.setFont(font);
    g.setColour(color);
    g.drawText(label, juce::Rectangle<int>{ 0, 2, knobWidth, captionHeight }, juce::Justification::centredTop);
}

void RotaryKnob::resized()
{
    slider.setBounds(0, captionHeight, knobWidth, knobHeight + textBoxHeight);
}
