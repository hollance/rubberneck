#include "RotaryKnob.h"

static constexpr float pi = juce::MathConstants<float>::pi;

static constexpr int captionH = 22;
static constexpr int textBoxH = 16;
static constexpr int sliderW = 80;
static constexpr int sliderH = sliderW;

RotaryKnob::RotaryKnob()
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, sliderW, textBoxH);
    slider.setRotaryParameters(1.25f * pi, 2.75f * pi, true);
    addAndMakeVisible(slider);

    setBounds(0, 0, sliderW, captionH + sliderH + textBoxH);
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
    g.drawText(label, juce::Rectangle<int>{ 0, 2, sliderW, captionH }, juce::Justification::centredTop);
}

void RotaryKnob::resized()
{
    slider.setBounds(0, captionH, sliderW, sliderH + textBoxH);
}
