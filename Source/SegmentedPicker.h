#pragma once

#include <JuceHeader.h>

class SegmentedPicker : public juce::Component, private juce::Button::Listener
{
public:
    SegmentedPicker(juce::AudioParameterChoice& parameter);
    ~SegmentedPicker() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setLookAndFeel(juce::LookAndFeel* newLookAndFeel);

    juce::TextButton allButton;
    juce::TextButton midsButton;
    juce::TextButton sidesButton;

private:
    juce::DropShadow dropShadow;

    void setValue(float newValue);
    void buttonClicked(juce::Button* button) override;

    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SegmentedPicker)
};
